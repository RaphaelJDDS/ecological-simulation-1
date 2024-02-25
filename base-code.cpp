#include <iostream>
#include <fstream>
#include <vector>
#include <random>

enum IndividualType { NICE, BULLY };

struct Individual {
    IndividualType type;
    int points;
    bool lastAction;  // true for cooperate, false for defect
};

int main() {
    int numberOfRuns = 1;  // Define the number of runs here
    double niceInteractionProbability = 0.25;  // Minimum probability of nice individuals interacting
    double forgivenessProbability = 0.1;  // Probability of forgiveness, used at 0.1 throughout the article (optimal config)
    std::ofstream file("population.csv"); 
    file << "Generation,Nice,Bullies\n";  

    for (int run = 0; run < numberOfRuns; ++run) {
        std::vector<Individual> population;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> disReal(0.0, 1.0);
        std::uniform_int_distribution<> dis2(0, 10);

        // Initialize population
        for (int i = 0; i < 200; ++i) {
            population.push_back({NICE, 0, true});
        }
        for (int i = 0; i < 200; ++i) {
            population.push_back({BULLY, 0, false});
        }
        
        // Simulation
        for (int i = 0; i < 100000; ++i) {  // Run for n iterations
            if (population.size() < 2) break;  // Not enough individuals for interaction

            int niceCount = 0, bullyCount = 0;    
            for (const auto &ind : population) {
                if (ind.type == NICE) {
                    ++niceCount;
                } 
                else {
                    ++bullyCount;
                }
            }
            std::uniform_int_distribution<> dis(0, niceCount + bullyCount);

            int idx1 = dis(gen) % population.size();
            int idx2 = dis(gen) % population.size();
            while (idx1 == idx2 || 
                   (population[idx1].type == NICE && population[idx2].type == NICE && disReal(gen) > niceInteractionProbability)) {
                idx2 = dis(gen) % population.size(); //Guarantee two separate individuals
            }

            Individual &ind1 = population[idx1];
            Individual &ind2 = population[idx2];

            bool action1, action2;

            // Decide actions
            if (ind1.type == BULLY) {
                action1 = false;  // Bully always defects
            } else {  // Nice
                if (ind2.lastAction || disReal(gen) < forgivenessProbability) {
                    action1 = true;  // Cooperate if the other cooperated last time or with forgiveness probability
                } else {
                    action1 = false;  // Otherwise defect
                }
            }

            if (ind2.type == BULLY) {
                action2 = false;  // Bully always defects
            } else {  // Nice
                if (ind1.lastAction || disReal(gen) < forgivenessProbability) {
                    action2 = true;  // Cooperate if the other cooperated last time or with forgiveness probability
                } else {
                    action2 = false;  // Otherwise defect
                }
            }

            // Update last actions
            ind1.lastAction = action1;
            ind2.lastAction = action2;

            // Interaction
            if (!action1 && !action2) {
                // Both defect
                ind1.points += 1;
                ind2.points += 1;
            } else if (action1 && action2) {
                // Both cooperate
                ind1.points += 3;
                ind2.points += 3;
            } else if (action1 && !action2) {
                // Nice cooperates, bully defects
                ind1.points += -5;
                ind2.points += 5;
            } else if (!action1 && action2) {
                // Bully defects, nice cooperates
                ind1.points += 5;
                ind2.points += -5;
            }

            // Survival
            if (ind1.points == 0) {
                if (dis2(gen) < 8) {  // 4/5 chance of survival
                    population.erase(population.begin() + idx1);
                    if (idx1 < idx2) --idx2;  // Adjust idx2 if necessary
                }
            }
            if (idx2 < population.size() && ind2.points == 0) {
                if (dis2(gen) < 8) {  // 4/5 chance of survival
                    population.erase(population.begin() + idx2);
                }
            }

            // Reproduction
            if (idx1 < population.size() && ind1.points >= 10) {
                population.push_back({ind1.type, 0, ind1.lastAction});
                ind1.points = 0;
            }
            if (idx2 < population.size() && ind2.points >= 10) {
                population.push_back({ind2.type, 0, ind2.lastAction});
                ind2.points = 0;
            }
            //Prints out the results for each generation. 
            std::cout << "Run " << run+1 << " (generation #" << i+1 << "): Nice: " << niceCount << ", Bullies: " << bullyCount << std::endl;
            //Saves the data in a given csv spreadsheet.
            file << i+1 << "," << niceCount << "," << bullyCount << ", \n";
        }
        int niceCount = 0, bullyCount = 0;    
        for (const auto &ind : population) {
            if (ind.type == NICE) {
                ++niceCount;
            } 
            else {
                ++bullyCount;
            }
        }
        // Print results
        std::cout << "Run " << run+1 << " (result): Nice: " << niceCount << ", Bullies: " << bullyCount << std::endl;
    }
    file.close();

    return 0;
}
