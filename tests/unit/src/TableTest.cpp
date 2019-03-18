#include <cstdint>
#include <vector>
#include <gtest/gtest.h>
#include <postgres/Connection.h>
#include <postgres/Visitable.h>

namespace postgres {

struct Table {
    int32_t n = 0;

    POSTGRES_CXX_TABLE("conn_test", n);
};

struct TableTest : testing::Test {
    TableTest() {
        conn_.create<Table>();
    }

    ~TableTest() noexcept override {
        conn_.drop<Table>();
    }

    Connection conn_;
};

TEST_F(TableTest, Insert) {
    Table in{};
    in.n = 1;
    ASSERT_TRUE(conn_.insert(in).isOk());

    std::vector<Table> out{};
    ASSERT_TRUE(conn_.select(out).isOk());
    ASSERT_EQ(1u, out.size());
    ASSERT_EQ(1, out[0].n);
}

TEST_F(TableTest, Update) {
    Table in{};
    in.n = 1;
    ASSERT_TRUE(conn_.insert(in).isOk());

    in.n = 2;
    ASSERT_TRUE(conn_.update(in).isOk());

    std::vector<Table> out{};
    ASSERT_TRUE(conn_.select(out).isOk());
    ASSERT_EQ(1u, out.size());
    ASSERT_EQ(2, out[0].n);
}

TEST_F(TableTest, MultiInsert) {
    std::vector<Table> in(3);
    in[0].n = 1;
    in[1].n = 2;
    in[2].n = 3;
    ASSERT_TRUE(conn_.insert(in.begin(), in.end()).isOk());

    std::vector<Table> out{};
    ASSERT_TRUE(conn_.select(out).isOk());
    ASSERT_EQ(3u, out.size());
    ASSERT_EQ(6, out[0].n + out[1].n + out[2].n);
}

TEST_F(TableTest, Select) {
    std::vector<Table> out{};
    ASSERT_TRUE(conn_.select(out).isOk());
    ASSERT_TRUE(out.empty());

    Table in{};
    ASSERT_TRUE(conn_.insert(in).isOk());

    ASSERT_TRUE(conn_.select(out).isOk());
    ASSERT_EQ(1u, out.size());
    ASSERT_TRUE(conn_.select(out).isOk());
    ASSERT_EQ(2u, out.size());
}

}  // namespace postgres
