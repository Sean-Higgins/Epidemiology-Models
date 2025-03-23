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
    let args: Args = env::args();
    for arg in args {
        println!("{arg}");
    }
    
    // If command-line arguments are provided, they are parsed in and stored with the appropriate flags:
    // IMPORTANT: We also need to handle error cases where a user
    // forgets to enter a value (e.g., ./influenzaOutbreak -s)
    // This leaves argv[argc-1] as the flag, and the final argv[argc] as NULL.
    if args.len() > 1 {
        
    }

    // Run the simulation for however many months the user wants.
    let mut max_months = 
}
