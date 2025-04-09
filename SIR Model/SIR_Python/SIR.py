### SIR Model Simulation in Python
# This program uses multithreading and parallel calculations
#          to simulate the progress of a flu outbreak on a human population
#          using the SIR epidemiology model.
#              (S)usceptible
#              (I)nfected
#              (R)ecovered
# Programmer: Sean B. Higgins
# Date: 2025-04-09

import threading
import argparse

# Create a barrier for synchronization between our threads
barrier = threading.Barrier(4)  # 4 threads: susceptible, infected, recovered, watcher

def susceptible():
    # Since this function will end up modifying the current_susceptible variable, we need to make sure that we are using the global variable
    # and not a local variable. This is done by using the global keyword.
    global current_susceptible
    
    # Since this function isn't changing any of the other variables, we don't need to use the global keyword for them.
    while now_days < max_days:
        # Calculate the next number of susceptible individuals using the SIR model formula
        # S(t+1) = S(t) - beta * S(t) * I(t) = S(t) * (1 - beta * I(t))
        next_susceptible = current_susceptible * (1 - beta * current_infected)

        # Check if the next number of susceptible individuals is less than 0 and set it to 0 if it is
        # This is to prevent negative values in the simulation
        if next_susceptible < 0:
            next_susceptible = 0
        
        # Done computing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Update the global variable
        current_susceptible = next_susceptible

        # Done assigning barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Done printing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()


def infected():
    # Since this function will end up modifying the current_infected variable, we need to make sure that we are using the global variable
    # and not a local variable. This is done by using the global keyword.
    global current_infected

    # Since this function isn't changing any of the other variables, we don't need to use the global keyword for them.

    while now_days < max_days:
        # Calculate the next number of infected individuals using the SIR model formula
        # I(t+1) = I(t) + beta * S(t) * I(t) - gamma * I(t) = (1 + beta * S(t) - gamma) * I(t)
        next_infected = (1 + beta * current_susceptible - gamma) * current_infected

        # Check if the next number of infected individuals is less than 0 and set it to 0 if it is
        # This is to prevent negative values in the simulation
        if next_infected < 0:
            next_infected = 0

        # Done computing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Update the global variable
        current_infected = next_infected

        # Done assigning barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Done printing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

def recovered():
    # Since this function will end up modifying the current_recovered variable, we need to make sure that we are using the global variable
    # and not a local variable. This is done by using the global keyword.
    global current_recovered

    # Since this function isn't changing any of the other variables, we don't need to use the global keyword for them.

    while now_days < max_days:
        # Calculate the next number of recovered individuals using the SIR model formula
        # R(t+1) = R(t) + (gamma * I(t))
        next_recovered = current_recovered + (gamma * current_infected)

        # Check if the next number of recovered individuals is less than 0 and set it to 0 if it is
        # This is to prevent negative values in the simulation
        if next_recovered < 0:
            next_recovered = 0
        
        # Done computing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Update the global variable
        current_recovered = next_recovered

        # Done assigning barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Done printing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

def watcher():
    # This function will be used to watch the simulation and print the current state of the simulation.
    # Since this function isn't changing any of the other variables, we don't need to use the global keyword for them.
    # However, this function will be updating the now_days variable, so we need to use the global keyword for it.
    global now_days

    while now_days < max_days:

        # Done computing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Done assigning barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()

        # Print the current state of the simulation
        print(f"Day: {now_days+1:3d}, Susceptible: {current_susceptible:8.0f}, Infected: {current_infected:8.0f}, Recovered: {current_recovered:8.0f}")
        now_days += 1

        # Done printing barrier
        # Wait for the other threads to finish before proceeding
        barrier.wait()


def main():
    global current_susceptible, current_infected, current_recovered
    global beta, gamma
    global max_days, now_days

    now_days = 0

    print("SIR Model Simulation")
    print("This program uses multithreading and parallel calculations")
    print("to simulate the progress of a flu outbreak on a human population")
    print("using the SIR epidemiology model.")

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='SIR Model Simulation')
    parser.add_argument('--days', type=int, default=100, help='Number of days to run the simulation')
    parser.add_argument('--susceptible', type=int, default=10000, help='Initial number of susceptible individuals')
    parser.add_argument('--infected', type=int, default=10, help='Initial number of infected individuals')
    parser.add_argument('--recovered', type=int, default=0, help='Initial number of recovered individuals')
    parser.add_argument('--beta', type=float, default=0.0001, help='Infection rate')
    parser.add_argument('--gamma', type=float, default=0.1, help='Recovery rate')
    args = parser.parse_args()

    # Initialize the population and parameters
    max_days = args.days
    current_susceptible = args.susceptible
    current_infected = args.infected
    current_recovered = args.recovered

    # Infection and recovery rates
    beta = args.beta  # Infection rate (beta)
    gamma = args.gamma  # Recovery rate (gamma)

    # Print the initial state of the simulation
    print(f"Day: {now_days+1:3d}, Susceptible: {current_susceptible:8.0f}, Infected: {current_infected:8.0f}, Recovered: {current_recovered:8.0f}")

    # Increment to the next day for our calculations
    now_days += 1

    # Create and start the threads for susceptible, infected, and recovered calculations as well as the watcher
    threads = []
    threads.append(threading.Thread(target=susceptible))
    threads.append(threading.Thread(target=infected))
    threads.append(threading.Thread(target=recovered))
    threads.append(threading.Thread(target=watcher))

    # With the threads created, we can start them
    for thread in threads:
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    # Print the final state of the simulation
    print(f"Final state after {max_days:3d} days:\nSusceptible: {current_susceptible:8.0f}, Infected: {current_infected:8.0f}, Recovered: {current_recovered:8.0f}")

if __name__ == "__main__":
    main()