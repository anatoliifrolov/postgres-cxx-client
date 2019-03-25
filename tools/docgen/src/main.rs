use std::env;
use std::fs;

fn main() {
    for filename in env::args().skip(1) {
        let contents = fs::read_to_string(&filename).unwrap();
        for line in contents.lines() {
            println!("{}", line.trim_left_matches("/// ").trim_left_matches("///"));
        }
    }
}
