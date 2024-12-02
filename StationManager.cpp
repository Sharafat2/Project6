/*
Sharafat Hussin
11/30/24
*/

#include "StationManager.hpp"
#include <iostream>
#include <algorithm>

/**
 * @file StationManager.cpp
 * @brief Implementation of the StationManager class managing kitchen stations, dishes, and ingredient stocks.
 */

// Default Constructor
StationManager::StationManager() {
    // Initializes an empty station manager
}

// Existing methods
// ...

// Adds a new station to the station manager
bool StationManager::addStation(KitchenStation* station) {
    return insert(item_count_, station);
}

// Removes a station from the station manager by name
bool StationManager::removeStation(const std::string& station_name) {
    for (int i = 0; i < item_count_; ++i) {
        if (getEntry(i)->getName() == station_name) {
            return remove(i);
        }
    }
    return false;
}

// Finds a station in the station manager by name
KitchenStation* StationManager::findStation(const std::string& station_name) const {
    Node<KitchenStation*>* searchptr = getHeadNode();
    while (searchptr != nullptr) {
        if (searchptr->getItem()->getName() == station_name) {
            return searchptr->getItem();
        }
        searchptr = searchptr->getNext();
    }
    return nullptr;
}

// Moves a specified station to the front of the station manager list
bool StationManager::moveStationToFront(const std::string& station_name) {
    // First, make sure the station exists
    if (findStation(station_name) == nullptr) {
        return false;
    }

    // If it's already at the front, return true
    if (getHeadNode()->getItem()->getName() == station_name) {
        return true;
    }

    Node<KitchenStation*>* searchptr = getHeadNode();
    while (searchptr != nullptr) {
        if (searchptr->getItem()->getName() == station_name) {
            // Make a copy of the station
            KitchenStation* station = searchptr->getItem();

            // Remove the station from its current position
            int pos = getStationIndex(searchptr->getItem()->getName());
            remove(pos);

            // Insert the station at the front
            insert(0, station);

            return true;  // Exit after moving the station
        }

        searchptr = searchptr->getNext();  // Move to the next node
    }

    return false;
}

int StationManager::getStationIndex(const std::string& name) const {
    Node<KitchenStation*>* searchptr = getHeadNode();
    int index = 0;
    while (searchptr != nullptr) {
        if (searchptr->getItem()->getName() == name) {
            return index;
        }
        searchptr = searchptr->getNext();
        index++;
    }
    return -1;
}

// Merges the dishes and ingredients of two specified stations
bool StationManager::mergeStations(const std::string& station_name1, const std::string& station_name2) {
    KitchenStation* station1 = findStation(station_name1);
    KitchenStation* station2 = findStation(station_name2);
    if (station1 && station2) {
        // take all the dishes from station2 and add them to station1
        for (Dish* dish : station2->getDishes()) {
            station1->assignDishToStation(dish);
        }
        // take all the ingredients from station2 and add them to station1
        for (Ingredient ingredient : station2->getIngredientsStock()) {
            station1->replenishStationIngredients(ingredient);
        }
        // remove station2 from the list
        removeStation(station_name2);
        return true;
    }
    return false;
}

// Assigns a dish to a specific station
bool StationManager::assignDishToStation(const std::string& station_name, Dish* dish) {
    KitchenStation* station = findStation(station_name);
    if (station) {
        return station->assignDishToStation(dish);
    }
    return false;
}

// Replenishes an ingredient at a specific station
bool StationManager::replenishIngredientAtStation(const std::string& station_name, const Ingredient& ingredient) {
    KitchenStation* station = findStation(station_name);
    if (station) {
        station->replenishStationIngredients(ingredient);
        return true;
    }
    return false;
}

// Checks if any station in the station manager can complete an order for a specific dish
bool StationManager::canCompleteOrder(const std::string& dish_name) const {
    Node<KitchenStation*>* searchptr = getHeadNode();
    while (searchptr != nullptr) {
        if (searchptr->getItem()->canCompleteOrder(dish_name)) {
            return true;
        }
        searchptr = searchptr->getNext();
    }
    return false;
}

// Prepares a dish at a specific station if possible
bool StationManager::prepareDishAtStation(const std::string& station_name, const std::string& dish_name) {
    KitchenStation* station = findStation(station_name);
    if (station && station->canCompleteOrder(dish_name)) {
        return station->prepareDish(dish_name);
    }
    return false;
}

// New methods implementation

std::queue<Dish*> StationManager::getDishQueue() const {
    return dish_queue_;
}

std::vector<Ingredient> StationManager::getBackupIngredients() const {
    return backup_ingredients_;
}

void StationManager::setDishQueue(const std::queue<Dish*>& dish_queue) {
    dish_queue_ = dish_queue;
}

void StationManager::addDishToQueue(Dish* dish) {
    if (dish != nullptr) {
        dish_queue_.push(dish);
    }
}

void StationManager::addDishToQueue(Dish* dish, const DietaryRequest& request) {
    if (dish != nullptr) {
        dish->applyDietaryRequest(request); // Adjust dish for dietary accommodations
        dish_queue_.push(dish);
    }
}

bool StationManager::prepareNextDish() {
    if (dish_queue_.empty()) {
        return false;
    }

    Dish* dish = dish_queue_.front();
    std::cout << "PREPARING DISH: " << dish->getName() << std::endl;
    bool dishPrepared = false;

    // Attempt to prepare the dish at each station in the list
    Node<KitchenStation*>* stationNode = getHeadNode();
    while (stationNode != nullptr) {
        KitchenStation* station = stationNode->getItem();
        std::cout << station->getName() << " attempting to prepare " << dish->getName() << "..." << std::endl;
        if (station->canPrepare(dish)) {
            bool success = station->prepareDish(dish->getName());
            if (success) {
                std::cout << station->getName() << ": Successfully prepared " << dish->getName() << "." << std::endl;
                dishPrepared = true;
                dish_queue_.pop();
                delete dish; // Free allocated memory
                break;
            } else {
                std::cout << station->getName() << ": Insufficient ingredients. Replenishing ingredients..." << std::endl;
                // Attempt to replenish ingredients
                bool replenished = true;
                for (const auto& ingredient : dish->getIngredients()) {
                    replenished &= replenishIngredient(station->getName(), ingredient.getName(), ingredient.getQuantity());
                }
                if (replenished) {
                    std::cout << station->getName() << ": Ingredients replenished." << std::endl;
                    success = station->prepareDish(dish->getName());
                    if (success) {
                        std::cout << station->getName() << ": Successfully prepared " << dish->getName() << "." << std::endl;
                        dishPrepared = true;
                        dish_queue_.pop();
                        delete dish; // Free allocated memory
                        break;
                    } else {
                        std::cout << station->getName() << ": Unable to prepare " << dish->getName() << " after replenishing." << std::endl;
                    }
                } else {
                    std::cout << station->getName() << ": Unable to replenish ingredients. Failed to prepare " << dish->getName() << "." << std::endl;
                }
            }
        } else {
            std::cout << station->getName() << ": Dish not available. Moving to next station..." << std::endl;
        }
        stationNode = stationNode->getNext();
    }

    if (!dishPrepared) {
        std::cout << dish->getName() << " was not prepared." << std::endl;
        // Dish remains in queue
        return false;
    }
    return true;
}

void StationManager::displayDishQueue() const {
    std::queue<Dish*> temp_queue = dish_queue_;
    while (!temp_queue.empty()) {
        Dish* dish = temp_queue.front();
        std::cout << dish->getName() << std::endl;
        temp_queue.pop();
    }
}

void StationManager::clearDishQueue() {
    while (!dish_queue_.empty()) {
        delete dish_queue_.front();
        dish_queue_.pop();
    }
}

bool StationManager::replenishStationIngredientFromBackup(const std::string& station_name, const std::string& ingredient_name, int quantity) {
    // Find the backup ingredient
    auto backup_it = std::find_if(backup_ingredients_.begin(), backup_ingredients_.end(),
        [&](const Ingredient& ingr) { return ingr.getName() == ingredient_name; });

    if (backup_it != backup_ingredients_.end() && backup_it->getQuantity() >= quantity) {
        // Find the station
        KitchenStation* station = findStation(station_name);
        if (station != nullptr) {
            // Add ingredient to station
            station->replenishStationIngredients(Ingredient(ingredient_name, quantity));
            // Reduce quantity from backup
            backup_it->setQuantity(backup_it->getQuantity() - quantity);
            if (backup_it->getQuantity() == 0) {
                backup_ingredients_.erase(backup_it);
            }
            return true;
        }
    }
    return false;
}

bool StationManager::addBackupIngredients(const std::vector<Ingredient>& ingredients) {
    backup_ingredients_ = ingredients;
    return true;
}

bool StationManager::addBackupIngredient(const Ingredient& ingredient) {
    auto it = std::find_if(backup_ingredients_.begin(), backup_ingredients_.end(),
        [&](const Ingredient& ingr) { return ingr.getName() == ingredient.getName(); });

    if (it != backup_ingredients_.end()) {
        it->setQuantity(it->getQuantity() + ingredient.getQuantity());
    } else {
        backup_ingredients_.push_back(ingredient);
    }
    return true;
}

void StationManager::clearBackupIngredients() {
    backup_ingredients_.clear();
}

void StationManager::processAllDishes() {
    size_t initialQueueSize = dish_queue_.size();
    for (size_t i = 0; i < initialQueueSize; ++i) {
        bool prepared = prepareNextDish();
        if (!prepared) {
            // Move dish to end of queue to keep original order for unprepared dishes
            Dish* dish = dish_queue_.front();
            dish_queue_.pop();
            dish_queue_.push(dish);
        }
    }
    std::cout << "All dishes have been processed." << std::endl;
}

// Helper Functions

KitchenStation* StationManager::findStationForDish(const Dish* dish) {
    Node<KitchenStation*>* searchptr = getHeadNode();
    while (searchptr != nullptr) {
        KitchenStation* station = searchptr->getItem();
        if (station->canPrepare(dish)) {
            return station;
        }
        searchptr = searchptr->getNext();
    }
    return nullptr;
}

bool StationManager::replenishIngredient(const std::string& station_name, const std::string& ingredient_name, int quantity) {
    return replenishStationIngredientFromBackup(station_name, ingredient_name, quantity);
}

bool StationManager::useBackupIngredient(const std::string& ingredient_name, int quantity) {
    auto backup_it = std::find_if(backup_ingredients_.begin(), backup_ingredients_.end(),
        [&](const Ingredient& ingr) { return ingr.getName() == ingredient_name; });

    if (backup_it != backup_ingredients_.end() && backup_it->getQuantity() >= quantity) {
        backup_it->setQuantity(backup_it->getQuantity() - quantity);
        if (backup_it->getQuantity() == 0) {
            backup_ingredients_.erase(backup_it);
        }
        return true;
    }
    return false;
}
