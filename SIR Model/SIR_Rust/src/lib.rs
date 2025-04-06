use std::sync::{Arc, Mutex};
use std::thread;

/// Calculates the number of susceptible individuals based on the current number of infected
/// individuals.
pub fn next_susceptible(
    susceptible: u64,
    infected: u64,
    infection_rate: f64
) -> u64 {
    // The number of susceptible individuals decreases by the infection rate,
    // as more susceptible people become infected.
    
    // Calculate the next number of susceptible individuals.
    let next_susceptible: u64 = (susceptible as f64 * (1.0 - infection_rate * (infected as f64))) as u64;
    println!("Next susceptible: {next_susceptible}");
    // Return the next number of susceptible individuals.
    next_susceptible
}

/// Calculates the number of infected individiduals based on the current number of susceptible and
/// infected individuals.
pub fn next_infected(
    susceptible: u64,
    infected: u64,
    infection_rate: f64,
    recovery_rate: f64,
) -> u64 {
    // The number of infected individuals increases by the infection rate as more
    // susceptible people become infected, but decreases based on the recovery rate
    // as more people recover.
    let next_infected: u64 = (infected as f64 * ((1.0 - recovery_rate) + (infection_rate * susceptible as f64))) as u64;
    println!("Next infected: {next_infected}");
    // Return the next number of infected individuals.
    next_infected
}

/// Calculates the number of recovered individuals based on the current number of infected
/// individuals.
pub fn next_recovered(
    infected: u64,
    recovered: u64,
    recovery_rate: f64,
) -> u64 {
    // The number of recovered individuals increased by the recovery rate as more
    // people recover
    let next_recovered: u64 = recovered + (recovery_rate * infected as f64) as u64;
    println!("Next recovered: {next_recovered}");
    // Return the next number of recovered individuals.
    next_recovered
}

/// Prints out the current population values for each step of the SIR model's simulation.
pub fn watcher(
    susceptible: u64,
    infected: u64,
    recovered: u64,
    month: i32,
) {
    let year: i32 = month / 12;
    let month: i32 = month % 12 + 1;
    println!("Year {year}, Month {month} - Susceptible: {susceptible}, Infected: {infected}, Recovered: {recovered}");
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn susceptible_test() {
        let susceptible: u64 = 10_000;
        let infected: u64 = 10;
        let infection_rate: f64 = 0.0001;

        // 0.0001 * 10_000 * 10 = 10
        // Next susceptible = 10_000 - 10 = 9_990
        assert_eq!(9_990, next_susceptible(susceptible, infected, infection_rate));
    }

    #[test]
    fn infected_test() {
        let susceptible: u64 = 10_000;
        let infection_rate: f64 = 0.0001;

        let infected: u64 = 10;
        let recovery_rate: f64 = 0.02;

        // (0.0001 * 10_000 * 10) = 10 new infected
        // (0.1 * 10) = 1 new recovered.
        // Total Infected: 10 + 10 - 1 = 19
        assert_eq!(19, next_infected(susceptible, infected, infection_rate, recovery_rate));
    }

    #[test]
    fn recovered_test() {
        let infected: u64 = 10;
        let recovery_rate: f64 = 0.1;
        let recovered: u64 = 0;

        // (0.1 * 10) = 1 newly recovered
        // 0 + 1 = 1
        assert_eq!(1, next_recovered(infected, recovered, recovery_rate));
    }

    #[test]
    fn step_test() {
        let susceptible: u64 = 10_000;

        let infection_rate: f64 = 0.0001;
        let infected: u64 = 10;

        let recovery_rate: f64 = 0.1;
        let recovered: u64 = 0;

        let month: i32 = 1;

        // This should be split up amongst the 3 threads. This will be implemented later.
        let next_susceptible: u64 = next_susceptible(susceptible, infected, infection_rate);
        let next_infected: u64 = next_infected(susceptible, infected, infection_rate, recovery_rate);
        let next_recovered: u64 = next_recovered(infected, recovered, recovery_rate);
        watcher(next_susceptible, next_infected, next_recovered, month);

        assert_eq!(9_990, next_susceptible);
        assert_eq!(19, next_infected);
        assert_eq!(1, next_recovered);
    }
}
