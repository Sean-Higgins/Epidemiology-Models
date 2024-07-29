use std::sync::{Arc, Mutex};
use std::thread;

/// Calculates the number of susceptible individuals based on the current number of infected
/// individuals.
pub fn susceptible(
    susceptible: u64,
    infection_rate: f64
) -> u64 {
    // The number of susceptible individuals decreases by the infection rate,
    // as more susceptible people become infected.
    
    // Calculate the next number of susceptible individuals.
    susceptible * (1.0 - infection_rate) as u64
}

/// Calculates the number of infected individiduals based on the current number of susceptible and
/// infected individuals.
pub fn infected(
    susceptible: u64,
    infected: u64,
    infection_rate: f64,
    recovery_rate: f64,
) -> u64 {
    // The number of infected individuals increases by the infection rate as more
    // susceptible people become infected, but decreases based on the recovery rate
    // as more people recover.
    (infected * (1.0 - recovery_rate)) + (susceptible * infection_rate) as u64
}

/// Calculates the number of recovered individuals based on the current number of infected
/// individuals.
pub fn recovered(
    infected: u64,
    recovered: u64,
    recovery_rate: f64,
) -> u64 {
    // The number of recovered individuals increased by the recovery rate as more
    // people recover
    recovered + (infected * recovery_rate) as u64
}

/// Prints out the current population values for each step of the SIR model's simulation.
pub fn watcher(
    susceptible: u64,
    infected: u64,
    recovered: u64,
    month: i32,
) {
    let year = month / 12;
    let month = month % 12 + 1;
    println!("Year {year}, Month {month} - Susceptible: {susceptible}, Infected: {infected}, Recovered: {recovered}");
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn susceptible_test() {
        let susceptible = 100_000;
        let infection_rate = 0.05;

        // 100_000 * 0.05 = 5_000
        // Next susceptible = 95_000
        assert_eq!(95_000, susceptible(susceptible, infection_rate));
    }

    #[test]
    fn infected_test() {
        let susceptible = 100_000;
        let infection_rate = 0.05;

        let infected = 25_000;
        let recovery_rate = 0.02;

        // (100_000 * 0.05) = 5_000 new infected
        // (25_000 * 0.02) = 500 new recovered.
        // Total Infected: 25_000 + 5_000 - 500 = 29_500
        assert_eq!(29_500, infected(susceptible, infected, infection_rate, recovery_rate));
    }

    #[test]
    fn recovered_test() {
        let infected = 25_000;
        let recovery_rate = 0.02;
        let recovered = 500;

        // (25_000 * 0.02) = 500 newly recovered
        // 500 + 500 = 1_000
        assert_eq!(1_000, recovered(infected, recovered, recovery_rate));
    }

    #[test]
    fn step_test() {
        let susceptible = 100_000;
        let infection_rate = 0.05;

        let infected = 25_000;
        let recovery_rate = 0.02;
        let recovered = 500;
        let month = 1;

        // This should be split up amongst the 3 threads. This will be implemented later.
        let next_susceptible = susceptible(susceptible, infection_rate);
        let next_infected = infected(susceptible, infected, infection_rate, recovery_rate);
        let next_recovered = recovered(infected, recovered, recovery_rate);
        watcher(next_susceptible, next_infected, next_recovered, month);

        assert_eq!(95_000, next_susceptible);
        assert_eq!(29_500, next_infected);
        assert_eq!(1_000, next_recovered);
    }
}
