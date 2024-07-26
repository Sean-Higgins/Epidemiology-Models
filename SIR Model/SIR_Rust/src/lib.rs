use std::sync::{Arc, Mutex};

/// Calculates the number of susceptible individuals based on the current number of infected
/// individuals.
pub fn susceptible(
    &mut susceptible: u64,
    infected: u64,
    infection_rate: f64
) {
    
}

/// Calculates the number of infected individiduals based on the current number of susceptible and
/// infected individuals.
pub fn infected(
    susceptible: u64,
    &mut infected: u64,
    recovered: u64,
    infection_rate: f64,
    recovery_rate: f64,
) {
    
}

/// Calculates the number of recovered individuals based on the current number of infected
/// individuals.
pub fn recovered(
    susceptible: u64,
    infected: u64,
    &mut recovered: u64,
    infection_rate: f64,
    recovery_rate: f64,
) {
    
}

/// Prints out the current population values for each step of the SIR model's simulation.
pub fn watcher() {}

pub fn add(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn multi_read_one_write() {
        let counter = Arc::new(Mutex::new(0));


    }
}
