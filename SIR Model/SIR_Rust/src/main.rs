use std::{
    env,
    sync::{Arc, Mutex},
};
use SIR_Rust::*;

// Global variables to hold the population values to be used
// and calculated by the multiple threads.


// 

fn main() {
    // Store the command-line arguments as an iterator to read each user-provided value.
    let args = env::args();
    for arg in args {
        println!("{arg}");
    }

    watcher();
}
