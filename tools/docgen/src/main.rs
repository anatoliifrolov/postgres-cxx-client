use std::env;
use std::fs;

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();
    for filename in &args {
        let content = fs::read_to_string(filename).unwrap();
        if filename.ends_with(".md") {
            println!("{}", content);
            continue;
        }

        let mut lines = content.lines().peekable();
        while let Some(line) = lines.next() {
            // Code example.
            if !line.starts_with("///") {
                println!("{}", line);
                continue;
            }

            // Markdown, table or empty line.
            let line = line.trim_left_matches("/// ").trim_left_matches("///");
            let as_is =
                line.is_empty() || line.contains("|") || !line.as_bytes()[0].is_ascii_alphabetic();
            if as_is {
                println!("{}", line);
                continue;
            }

            // Piece of text.
            match lines.peek() {
                None => {
                    println!("{}", line);
                }
                Some(&next) => {
                    let is_continuing = 4 < next.len()
                        && next.starts_with("/// ")
                        && next.as_bytes()[4].is_ascii_alphabetic()
                        && !next.contains("|");
                    if is_continuing {
                        print!("{} ", line);
                    } else {
                        println!("{}", line);
                    }
                }
            }
        }
    }
}
