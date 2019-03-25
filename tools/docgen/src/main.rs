use std::env;
use std::fs;

fn main() {
    let mut buf = String::new();
    let mut toc = String::from("## Table of Contents\n\n");
    for filename in env::args().skip(1) {
        for line in fs::read_to_string(&filename).unwrap().lines() {
            // Handles both '///' and '/// blah-blah-blah'.
            let line = line.trim_left_matches("/// ").trim_left_matches("///");
            if !line.starts_with("##") {
                buf += line;
                buf += "\n";
                continue;
            }

            let chunks: Vec<&str> = line.split("## ").collect();
            let tag = chunks[1].to_lowercase().replace(" ", "-");
            toc += format!(
                "{}* [{}](#{})\n",
                chunks[0].replace("#", "  "),
                chunks[1],
                tag
            )
            .as_str();
            buf += format!("<a name=\"{}\"/>\n\n{}\n", tag, line).as_str();
        }
        buf += "\n";
    }
    toc += "\n";
    buf.insert_str(buf.find("<a name=").unwrap(), toc.as_str());
    print!("{}", buf);
}
