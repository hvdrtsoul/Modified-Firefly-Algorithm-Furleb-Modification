#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_PARTICLES 100
#define MAX_DIMENSIONS 10

float objective_function(float *x, int dimensions_number)
{
   return 280 - ((pow(x[0], 4.0) - 16 * pow(x[0], 2.0) + 5 * x[0]) / 2 + (pow(x[1], 4.0) - 16 * pow(x[1], 2.0) + 5 * x[1]) / 2);
}

typedef struct Result {
     float* best_solution;
     float best_intensity;
} Result;

// gcc -Wall -Wpedantic main.c -shared -o libcode.dll

Result firefly_algorithm(float (*function)(float *, int), int dimensions, float bounds_array[MAX_DIMENSIONS][2], int fireflies_number, 
                        int max_iteration, float alpha, float beta, int random_movement_possibilites)
{
    srand((unsigned) time(NULL));
    float** fireflies = (float**)malloc(fireflies_number * sizeof(float*));
    for(int i = 0;i < fireflies_number;++i)
        fireflies[i] = (float*)malloc(dimensions * sizeof(float));
    
    for(int firefly_number = 0;firefly_number < fireflies_number;++firefly_number)
        for(int dimension_number = 0;dimension_number < dimensions;++dimension_number)
            fireflies[firefly_number][dimension_number] = bounds_array[dimension_number][0] + ((float) rand() / (float) RAND_MAX) * (bounds_array[dimension_number][1] - bounds_array[dimension_number][0]);
    
    float* intensities = (float*)malloc(fireflies_number * sizeof(float));

    for(int firefly_number = 0;firefly_number < fireflies_number;++firefly_number)
        intensities[firefly_number] = function(fireflies[firefly_number], dimensions);

    // Initialized the initial swarm

    for(int iteration = 0;iteration < max_iteration;++iteration)
    {
        for(int i = 0;i < fireflies_number;++i)
        {
            int has_moved = 0; // Boolean alternative
            for(int j = 0;j < fireflies_number;++j)
            {
                if(intensities[j] > intensities[i]) // Moving firefly
                {
                    float distance = 0.0;

                    for(int dimension_number = 0;dimension_number < dimensions;++dimension_number)
                    {
                        distance += pow(fireflies[i][dimension_number] - fireflies[j][dimension_number], 2);
                    }

                    float a0;
                    
                    if(intensities[i] != 0.0)
                        a0 = intensities[j] / intensities[i];
                    else
                        a0 = 1.0;

                    a0 *= exp((-1) * beta * distance);

                    float* new_position = (float*)malloc(dimensions * sizeof(float));

                    for(int dimension_number = 0;dimension_number < dimensions;++dimension_number)
                    {
                        new_position[dimension_number] = fireflies[i][dimension_number];
                        new_position[dimension_number] += a0* (fireflies[j][dimension_number] - fireflies[i][dimension_number]);
                        new_position[dimension_number] += alpha * ((float) rand() / (float) RAND_MAX);

                        // Clamping:
                        if(new_position[dimension_number] < bounds_array[dimension_number][0])
                            new_position[dimension_number] = bounds_array[dimension_number][0];
                        else if(new_position[dimension_number] > bounds_array[dimension_number][1])
                            new_position[dimension_number] = bounds_array[dimension_number][1];
                    } // New position

                    fireflies[i] = new_position;
                    intensities[i] = function(new_position, dimensions);
                    has_moved = 1;
                }
            }

            if(!has_moved)
            {
                float* new_possible_position = (float*)malloc(dimensions * sizeof(float));
                for(int random_try = 0;random_try < random_movement_possibilites;++random_try)
                {
                    for(int dimension_number = 0;dimension_number < dimensions;++dimension_number)
                    {
                        new_possible_position[dimension_number] = fireflies[i][dimension_number];
                        new_possible_position[dimension_number] += alpha * ((float) rand() / (float) RAND_MAX);
                    }
                    float new_possible_intensity = function(new_possible_position, dimensions);

                    if(new_possible_intensity > intensities[i])
                    {
                        fireflies[i] = new_possible_position;
                        intensities[i] = new_possible_intensity;
                        break; // Found a direction where intensity will increase while random movement
                    }
                }
            }
        }
    }
    float best_shining = intensities[0];
        float* best_firefly = fireflies[0];

        for(int i = 0;i < fireflies_number;++i)
        {
            if(intensities[i] > best_shining)
            {
                best_shining = intensities[i];
                best_firefly = fireflies[i];
            }
        }

        Result to_return;
        to_return.best_solution = best_firefly;
        to_return.best_intensity = best_shining;
        
        return to_return;
}

int main() {
    srand((unsigned) time(NULL));
    float bonds[MAX_DIMENSIONS][2] = {{-5.0f, 5.0f}, {-5.0f, 5.0f}};
    Result result = firefly_algorithm(objective_function, 2, bonds, 100, 500, 0.4, 0.3, 100);
    
    printf("Best intensity is: %f\n", result.best_intensity);
    
    
    for(int i = 0;i < 2;++i)
        printf("%f ", result.best_solution[i]);
    
    return 0;
}
