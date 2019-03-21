use std::env;
use std::fs;

fn main() {
    let mut args: Vec<String> = env::args().skip(1).collect();
    for filename in &args {
        let content= fs::read_to_string(filename).unwrap();
        if filename.ends_with(".md") {
            println!("{}", content);
            continue;
        }

        let mut lines= content.lines().peekable();
        while let Some(line) = lines.next() {
            // Code example.
            if !line.starts_with("///") {
                println!("{}", line);
                continue;
            }

            // Markdown or empty line.
            let line = line.trim_left_matches("///").trim_left();
            if line.is_empty() || !line.as_bytes()[0].is_ascii_alphabetic() {
                println!("{}", line);
                continue;
            }

            // Piece of text.
            match lines.peek() {
                None => {
                    println!("{}", line);
                },
                Some(&next) => {
                    if 4 < next.len() && next.starts_with("/// ") && next.as_bytes()[4].is_ascii_alphabetic() {
                        print!("{} ", line);
                    } else {
                        println!("{}", line);
                    }
                }
            }
        }
    }
}
