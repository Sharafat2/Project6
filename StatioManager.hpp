/*
Sharafat Hussin
11/30/24
*/

#ifndef STATIONMANAGER_HPP
#define STATIONMANAGER_HPP

#include "LinkedList.hpp"
#include "KitchenStation.hpp"
#include "Dish.hpp"
#include "Appetizer.hpp"
#include "MainCourse.hpp"
#include "Dessert.hpp"
#include <queue>
#include <vector>
#include <string>

/**
 * @file StationManager.hpp
 * @brief Declaration of the StationManager class managing kitchen stations, dishes, and ingredient stocks.
 * @author
 * @date
 */

class StationManager : public LinkedList<KitchenStation*> {
public:
    /**
     * Default Constructor
     * @post: Initializes an empty station manager.
     */
    StationManager();

    // Existing methods
    // ...

    /**
     * Retrieves the current dish preparation queue.
     * @return A copy of the queue containing pointers to Dish objects.
     * @post: The dish preparation queue is returned unchanged.
     */
    std::queue<Dish*> getDishQueue() const;

    /**
     * Retrieves the list of backup ingredients.
     * @return A vector containing Ingredient objects representing backup supplies.
     * @post: The list of backup ingredients is returned unchanged.
     */
    std::vector<Ingredient> getBackupIngredients() const;

    /**
     * Sets the current dish preparation queue.
     * @param dish_queue A queue containing pointers to Dish objects.
     * @pre: The dish_queue contains valid pointers to dynamically allocated Dish objects.
     * @post: The dish preparation queue is replaced with the provided queue.
     */
    void setDishQueue(const std::queue<Dish*>& dish_queue);

    /**
     * Adds a dish to the preparation queue without dietary accommodations.
     * @param dish A pointer to a dynamically allocated Dish object.
     * @pre: The dish pointer is not null.
     * @post: The dish is added to the end of the queue.
     */
    void addDishToQueue(Dish* dish);

    /**
     * Adds a dish to the preparation queue with dietary accommodations.
     * @param dish A pointer to a dynamically allocated Dish object.
     * @param request A DietaryRequest object specifying dietary accommodations.
     * @pre: The dish pointer is not null.
     * @post: The dish is adjusted for dietary accommodations and added to the end of the queue.
     */
    void addDishToQueue(Dish* dish, const DietaryRequest& request);

    /**
     * Prepares the next dish in the queue if possible.
     * @pre: The dish queue is not empty.
     * @post: The dish is processed and removed from the queue.
     *       If the dish cannot be prepared, it stays in the queue.
     * @return: True if the dish was prepared successfully; false otherwise.
     */
    bool prepareNextDish();

    /**
     * Displays all dishes in the preparation queue.
     * @pre: None.
     * @post: Outputs the names of the dishes in the queue in order (each name is on its own line).
     */
    void displayDishQueue() const;

    /**
     * Clears all dishes from the preparation queue.
     * @pre: None.
     * @post: The dish queue is emptied and all allocated memory is freed.
     */
    void clearDishQueue();

    /**
     * Replenishes a specific ingredient at a given station from the backup ingredients stock by a specified quantity.
     * @param station_name A string representing the name of the station.
     * @param ingredient_name A string representing the name of the ingredient to replenish.
     * @param quantity An integer representing the amount to replenish.
     * @pre None.
     * @post If the ingredient is found in the backup ingredients stock and has sufficient quantity, it is added to the station's ingredient stock by the specified amount, and the function returns true.
     *       The quantity of the ingredient in the backup stock is decreased by the specified amount.
     *       If the ingredient in backup stock is depleted (quantity becomes zero), it is removed from the backup stock.
     *       If the ingredient does not have sufficient quantity in backup stock, or the ingredient or station is not found, returns false.
     * @return True if the ingredient was replenished from backup; false otherwise.
     */
    bool replenishStationIngredientFromBackup(const std::string& station_name, const std::string& ingredient_name, int quantity);

    /**
     * Sets the backup ingredients stock with the provided list of ingredients.
     * @param ingredients A vector of Ingredient objects to set as the backup stock.
     * @pre None.
     * @post The backup_ingredients_ vector is replaced with the provided ingredients.
     * @return True if the ingredients were added; false otherwise.
     */
    bool addBackupIngredients(const std::vector<Ingredient>& ingredients);

    /**
     * Adds a single ingredient to the backup ingredients stock.
     * @param ingredient An Ingredient object to add to the backup stock.
     * @pre None.
     * @post If the ingredient already exists in the backup stock, its quantity is increased by the ingredient's quantity.
     *       If the ingredient does not exist, it is added to the backup stock.
     * @return True if the ingredient was added; false otherwise.
     */
    bool addBackupIngredient(const Ingredient& ingredient);

    /**
     * Empties the backup ingredients vector.
     * @post The backup_ingredients_ private member variable is empty.
     */
    void clearBackupIngredients();

    /**
     * Processes all dishes in the queue and displays detailed results.
     * @pre: None.
     * @post: All dishes are processed, and detailed information is displayed
     *        (as per the format in the specifications), including station replenishments
     *        and preparation results.
     *        If a dish cannot be prepared even after replenishing ingredients, it
     *        stays in the queue in its original order.
     */
    void processAllDishes();

private:
    /**
     * Queue storing pointers to dishes to be prepared.
     */
    std::queue<Dish*> dish_queue_;

    /**
     * Backup stock of ingredients.
     */
    std::vector<Ingredient> backup_ingredients_;

    // Helper function to get index of a station by name
    int getStationIndex(const std::string& station_name) const;

    /**
     * Finds a station capable of preparing the given dish.
     * @param dish A pointer to the dish to be prepared.
     * @return A pointer to the KitchenStation that can prepare the dish; nullptr if none found.
     */
    KitchenStation* findStationForDish(const Dish* dish);

    /**
     * Replenishes an ingredient for a station from the backup ingredients.
     * @param station_name Name of the station.
     * @param ingredient_name Name of the ingredient to replenish.
     * @param quantity Quantity to replenish.
     * @return True if replenishment was successful; false otherwise.
     */
    bool replenishIngredient(const std::string& station_name, const std::string& ingredient_name, int quantity);

    /**
     * Helper function to check and adjust backup ingredients.
     * @param ingredient_name Name of the ingredient.
     * @param quantity Quantity required.
     * @return True if the backup has sufficient quantity; false otherwise.
     */
    bool useBackupIngredient(const std::string& ingredient_name, int quantity);
};

#endif // STATIONMANAGER_HPP
