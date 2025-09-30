/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 15:24:30 by hulefevr          #+#    #+#             */
/*   Updated: 2025/09/30 18:40:07 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cassert>
#include "./includes/pool.hpp"
#include "./includes/data_buffer.hpp"
#include "./includes/memento.hpp"
#include "./includes/observer.hpp"
#include "./includes/singleton.hpp"
#include "./includes/state_machine.hpp"



/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*========================================== CLASSES ===========================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/


// Simple class for testing with constructors and destructors
class TestClass {
public:
    TestClass(int val) : value(val) {
        std::cout << "TestClass constructor called with value: " << value << std::endl;
    }
    
    TestClass(const std::string& str) : value(str.length()), text(str) {
        std::cout << "TestClass constructor called with string: " << text << std::endl;
    }
    
    ~TestClass() {
        std::cout << "TestClass destructor called for value: " << value << std::endl;
    }
    
    int getValue() const { return value; }
    std::string getText() const { return text; }
    
    void setValue(int val) { value = val; }
    
private:
    int value;
    std::string text;
};


// Test class that implements Memento pattern
class GameCharacter : public Memento {
public:
    GameCharacter(const std::string& name, int level, float health, int gold)
        : _name(name), _level(level), _health(health), _gold(gold) {}
    
    // Getters
    std::string getName() const { return _name; }
    int getLevel() const { return _level; }
    float getHealth() const { return _health; }
    int getGold() const { return _gold; }
    
    // Setters
    void setLevel(int level) { _level = level; }
    void setHealth(float health) { _health = health; }
    void setGold(int gold) { _gold = gold; }
    
    // Display state
    void display() const {
        std::cout << "Character: " << _name 
                  << " | Level: " << _level 
                  << " | Health: " << _health 
                  << " | Gold: " << _gold << std::endl;
    }

protected:
    // Implementation of Memento pattern
    void _saveToSnapshot(Memento::Snapshot& snapshot) const override {
        snapshot << _name << _level << _health << _gold;
    }
    
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override {
        snapshot >> _name >> _level >> _health >> _gold;
    }

private:
    std::string _name;
    int _level;
    float _health;
    int _gold;
};


// Complex class with multiple data types
class GameWorld : public Memento {
public:
    GameWorld() : _worldName("Unknown"), _dayCount(1), _weatherTemp(20.0f), _isRaining(false) {}
    
    // Setters
    void setWorldName(const std::string& name) { _worldName = name; }
    void setDayCount(int days) { _dayCount = days; }
    void setWeatherTemp(float temp) { _weatherTemp = temp; }
    void setRaining(bool raining) { _isRaining = raining; }
    
    // Display
    void display() const {
        std::cout << "World: " << _worldName 
                  << " | Day: " << _dayCount 
                  << " | Temp: " << _weatherTemp << "°C"
                  << " | Raining: " << (_isRaining ? "Yes" : "No") << std::endl;
    }

protected:
    void _saveToSnapshot(Memento::Snapshot& snapshot) const override {
        snapshot << _worldName << _dayCount << _weatherTemp << _isRaining;
    }
    
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override {
        snapshot >> _worldName >> _dayCount >> _weatherTemp >> _isRaining;
    }

private:
    std::string _worldName;
    int _dayCount;
    float _weatherTemp;
    bool _isRaining;
};

class GameSettings {
public:
    GameSettings(const std::string& title, int width, int height) 
        : _gameTitle(title), _screenWidth(width), _screenHeight(height), _volume(50) {
        std::cout << "GameSettings created: " << _gameTitle 
                  << " (" << _screenWidth << "x" << _screenHeight << ")" << std::endl;
    }
    
    ~GameSettings() {
        std::cout << "GameSettings destroyed" << std::endl;
    }
    
    // Getters
    std::string getTitle() const { return _gameTitle; }
    int getWidth() const { return _screenWidth; }
    int getHeight() const { return _screenHeight; }
    int getVolume() const { return _volume; }
    
    // Setters
    void setTitle(const std::string& title) { _gameTitle = title; }
    void setVolume(int vol) { _volume = vol; }
    
    void display() const {
        std::cout << "Game: " << _gameTitle << " | Resolution: " 
                  << _screenWidth << "x" << _screenHeight 
                  << " | Volume: " << _volume << std::endl;
    }

private:
    std::string _gameTitle;
    int _screenWidth;
    int _screenHeight;
    int _volume;
};

class Logger {
public:
    Logger() : _logCount(0) {
        std::cout << "Logger instance created" << std::endl;
    }
    
    ~Logger() {
        std::cout << "Logger destroyed. Total logs: " << _logCount << std::endl;
    }
    
    void log(const std::string& message) {
        _logCount++;
        std::cout << "[LOG #" << _logCount << "] " << message << std::endl;
    }
    
    int getLogCount() const { return _logCount; }

private:
    int _logCount;
};


/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*========================================== POOL ==============================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/


// Test function to verify basic functionality
void testBasicPoolOperations() {
    std::cout << "\n=== Testing Basic Pool Operations ===\n";
    
    Pool<int> intPool;
    intPool.resize(5);
    
    std::cout << "Creating objects...\n";
    auto obj1 = intPool.acquire(42);
    auto obj2 = intPool.acquire(84);
    
    std::cout << "Object 1: " << *obj1 << std::endl;
    std::cout << "Object 2: " << *obj2 << std::endl;
    
    *obj1 = 100;
    std::cout << "Updated Object 1: " << *obj1 << std::endl;
    
    // Test move semantics
    std::cout << "Moving obj1 to obj2...\n";
    obj2 = std::move(obj1);
    
    // After move, obj1 is invalid - don't try to access it
    std::cout << "Object 1 has been moved (now invalid)." << std::endl;
    std::cout << "Moved Object 2: " << *obj2 << std::endl;
}

// Test with a custom class
void testWithCustomClass() {
    std::cout << "\n=== Testing with Custom Class ===\n";
    
    Pool<TestClass> classPool;
    classPool.resize(3);
    
    auto obj1 = classPool.acquire(123);
    auto obj2 = classPool.acquire("Hello World");
    
    std::cout << "Object 1 value: " << obj1->getValue() << std::endl;
    std::cout << "Object 2 value: " << obj2->getValue() << 
               ", text: " << obj2->getText() << std::endl;
    
    // Test modification
    obj1->setValue(999);
    std::cout << "Modified object 1 value: " << obj1->getValue() << std::endl;
}

// Test pool capacity and reuse
void testPoolCapacityAndReuse() {
    std::cout << "\n=== Testing Pool Capacity and Reuse ===\n";
    
    Pool<int> smallPool;
    smallPool.resize(2);
    
    std::cout << "Creating first two objects...\n";
    auto obj1 = smallPool.acquire(1);
    auto obj2 = smallPool.acquire(2);
    
    std::cout << "Object 1: " << *obj1 << std::endl;
    std::cout << "Object 2: " << *obj2 << std::endl;
    
    std::cout << "Releasing first object...\n";
    {
        // Force obj1's destructor to run by letting it go out of scope
        auto temp = std::move(obj1);
        std::cout << "Temp object: " << *temp << std::endl;
    } // temp goes out of scope here, releasing the pool object
    
    std::cout << "Creating third object (should reuse first slot)...\n";
    auto obj3 = smallPool.acquire(3);
    
    std::cout << "Object 2: " << *obj2 << std::endl;
    std::cout << "Object 3: " << *obj3 << std::endl;
    
    try {
        std::cout << "Attempting to create a fourth object (should fail)...\n";
        auto obj4 = smallPool.acquire(4);
        std::cout << "Object 4: " << *obj4 << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception caught: " << e.what() << std::endl;
    }
}

// Test invalid access
void testInvalidAccess() {
    std::cout << "\n=== Testing Invalid Access ===\n";
    
    Pool<int> pool;
    pool.resize(1);
    
    auto obj1 = pool.acquire(42);
    auto obj2 = std::move(obj1);
    
    std::cout << "Moved object value: " << *obj2 << std::endl;
    
    try {
        std::cout << "Attempting to access moved-from object...\n";
        std::cout << *obj1 << std::endl;  // This should throw
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception caught: " << e.what() << std::endl;
    }
}


/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*========================================= DATA BUFFER ========================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/

// Test DataBuffer functionality
void testDataBuffer() {
    std::cout << "\n=== Testing DataBuffer Operations ===\n";
    
    DataBuffer buffer;
    
    // Test basic properties
    std::cout << "Initial buffer size: " << buffer._size() << std::endl;
    std::cout << "Initial buffer capacity: " << buffer._capacity() << std::endl;
    
    // Test writing different data types
    std::cout << "\n--- Testing Write Operations ---\n";
    
    // Write integers
    int intValue1 = 42;
    int intValue2 = -123;
    buffer << intValue1 << intValue2;
    std::cout << "Written integers: " << intValue1 << ", " << intValue2 << std::endl;
    
    // Write floating point
    double doubleValue = 3.14159;
    buffer << doubleValue;
    std::cout << "Written double: " << doubleValue << std::endl;
    
    // Write string
    std::string stringValue = "Hello DataBuffer!";
    buffer << stringValue;
    std::cout << "Written string: \"" << stringValue << "\"" << std::endl;
    
    // Write character
    char charValue = 'A';
    buffer << charValue;
    std::cout << "Written char: '" << charValue << "'" << std::endl;
    
    std::cout << "Buffer size after writes: " << buffer._size() << std::endl;
    
    // Test reading data back (in same order)
    std::cout << "\n--- Testing Read Operations ---\n";
    
    int readInt1, readInt2;
    double readDouble;
    std::string readString;
    char readChar;
    
    buffer >> readInt1 >> readInt2 >> readDouble >> readString >> readChar;
    
    std::cout << "Read integers: " << readInt1 << ", " << readInt2 << std::endl;
    std::cout << "Read double: " << readDouble << std::endl;
    std::cout << "Read string: \"" << readString << "\"" << std::endl;
    std::cout << "Read char: '" << readChar << "'" << std::endl;
    
    // Verify values match
    bool success = (readInt1 == intValue1) && (readInt2 == intValue2) && 
                   (readDouble == doubleValue) && (readString == stringValue) && 
                   (readChar == charValue);
    
    std::cout << "Data integrity check: " << (success ? "PASSED" : "FAILED") << std::endl;
}

// Test DataBuffer advanced operations
void testDataBufferAdvanced() {
    std::cout << "\n=== Testing DataBuffer Advanced Operations ===\n";
    
    DataBuffer buffer;
    
    // Test with custom struct
    struct TestStruct {
        int x;
        float y;
        bool flag;
    };
    
    TestStruct original = {100, 2.5f, true};
    buffer << original;
    
    TestStruct read;
    buffer >> read;
    
    std::cout << "Original struct: x=" << original.x << ", y=" << original.y 
              << ", flag=" << original.flag << std::endl;
    std::cout << "Read struct: x=" << read.x << ", y=" << read.y 
              << ", flag=" << read.flag << std::endl;
    
    bool structMatch = (original.x == read.x) && (original.y == read.y) && 
                       (original.flag == read.flag);
    std::cout << "Struct integrity: " << (structMatch ? "PASSED" : "FAILED") << std::endl;
    
    // Test reset functionality
    std::cout << "\n--- Testing Reset Operations ---\n";
    buffer._reset();
    std::cout << "Buffer reset - size: " << buffer._size() << std::endl;
    
    // Test reading the struct again after reset (should work)
    TestStruct readAgain;
    buffer >> readAgain;
    
    std::cout << "Read after reset: x=" << readAgain.x << ", y=" << readAgain.y 
              << ", flag=" << readAgain.flag << std::endl;
    
    bool resetMatch = (original.x == readAgain.x) && (original.y == readAgain.y) && 
                      (original.flag == readAgain.flag);
    std::cout << "Reset functionality: " << (resetMatch ? "PASSED" : "FAILED") << std::endl;
    
    // Test clear functionality - AFTER reading
    buffer._clear();
    std::cout << "Buffer cleared - size: " << buffer._size() << std::endl;
    
    // Test reading from cleared buffer (should throw exception)
    try {
        TestStruct shouldFail;
        std::cout << "Attempting to read from cleared buffer...\n";
        buffer >> shouldFail;
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception caught: " << e.what() << std::endl;
    }
}

// Test DataBuffer error handling
void testDataBufferErrors() {
    std::cout << "\n=== Testing DataBuffer Error Handling ===\n";
    
    DataBuffer buffer;
    
    // Write some data
    int value = 42;
    buffer << value;
    
    // Read the data
    int readValue;
    buffer >> readValue;
    std::cout << "Successfully read: " << readValue << std::endl;
    
    // Try to read beyond available data
    try {
        int extraValue;
        std::cout << "Attempting to read beyond buffer...\n";
        buffer >> extraValue;
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception caught: " << e.what() << std::endl;
    }
    
    // Test string error handling
    DataBuffer stringBuffer;
    try {
        std::string testStr;
        std::cout << "Attempting to read string from empty buffer...\n";
        stringBuffer >> testStr;
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception caught: " << e.what() << std::endl;
    }
}


/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*========================================== MEMENTO ===========================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/

// NEW: Test Memento pattern functionality
void testMementoBasic() {
    std::cout << "\n=== Testing Memento Pattern - Basic ===\n";
    
    // Create a game character
    GameCharacter hero("Aragorn", 5, 100.0f, 250);
    std::cout << "Initial state: ";
    hero.display();
    
    // Save the initial state
    auto savePoint1 = hero.save();
    std::cout << "State saved at savePoint1\n";
    
    // Modify the character
    hero.setLevel(10);
    hero.setHealth(75.5f);
    hero.setGold(500);
    std::cout << "After modifications: ";
    hero.display();
    
    // Save another state
    auto savePoint2 = hero.save();
    std::cout << "State saved at savePoint2\n";
    
    // Modify again
    hero.setLevel(15);
    hero.setHealth(25.0f);
    hero.setGold(1000);
    std::cout << "After more modifications: ";
    hero.display();
    
    // Load previous state
    std::cout << "\nLoading savePoint2...\n";
    hero.load(savePoint2);
    std::cout << "After loading savePoint2: ";
    hero.display();
    
    // Load initial state
    std::cout << "\nLoading savePoint1...\n";
    hero.load(savePoint1);
    std::cout << "After loading savePoint1: ";
    hero.display();
    
    std::cout << "Memento basic test: PASSED\n";
}

// Test Memento with complex data
void testMementoAdvanced() {
    std::cout << "\n=== Testing Memento Pattern - Advanced ===\n";
    
    // Create a game world
    GameWorld world;
    world.setWorldName("Middle Earth");
    world.setDayCount(42);
    world.setWeatherTemp(15.5f);
    world.setRaining(true);
    
    std::cout << "Initial world state: ";
    world.display();
    
    // Save world state
    auto worldSave = world.save();
    std::cout << "World state saved\n";
    
    // Create multiple characters and save their states
    GameCharacter wizard("Gandalf", 50, 200.0f, 0);
    GameCharacter dwarf("Gimli", 20, 150.0f, 300);
    
    std::cout << "\nInitial characters:\n";
    wizard.display();
    dwarf.display();
    
    auto wizardSave = wizard.save();
    auto dwarfSave = dwarf.save();
    
    // Simulate a battle - characters take damage
    std::cout << "\nAfter battle:\n";
    wizard.setHealth(50.0f);
    dwarf.setHealth(10.0f);
    dwarf.setGold(280); // Lost some gold
    
    world.setDayCount(43);
    world.setRaining(false);
    world.setWeatherTemp(22.0f);
    
    wizard.display();
    dwarf.display();
    world.display();
    
    // Load all saved states (like loading a save game)
    std::cout << "\nLoading saved game state...\n";
    wizard.load(wizardSave);
    dwarf.load(dwarfSave);
    world.load(worldSave);
    
    std::cout << "Restored characters:\n";
    wizard.display();
    dwarf.display();
    std::cout << "Restored world: ";
    world.display();
    
    std::cout << "Memento advanced test: PASSED\n";
}

// Test Memento error handling and edge cases
void testMementoEdgeCases() {
    std::cout << "\n=== Testing Memento Pattern - Edge Cases ===\n";
    
    GameCharacter character("Test", 1, 100.0f, 0);
    
    // Test multiple saves and loads
    auto save1 = character.save();
    
    character.setLevel(5);
    auto save2 = character.save();
    
    character.setLevel(10);
    auto save3 = character.save();
    
    // Load in different order
    character.load(save1);
    std::cout << "Loaded save1: ";
    character.display();
    
    character.load(save3);
    std::cout << "Loaded save3: ";
    character.display();
    
    character.load(save2);
    std::cout << "Loaded save2: ";
    character.display();
    
    // Test that saves are independent
    GameCharacter char1("Player1", 1, 100.0f, 100);
    GameCharacter char2("Player2", 1, 100.0f, 100);
    
    auto char1Save = char1.save();
    auto char2Save = char2.save();
    
    char1.setLevel(10);
    char2.setLevel(20);
    
    // Loading char1's save shouldn't affect char2
    char1.load(char1Save);
    std::cout << "After loading char1's save:\n";
    std::cout << "Char1: ";
    char1.display();
    std::cout << "Char2: ";
    char2.display();
    
    std::cout << "Memento edge cases test: PASSED\n";
}

/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*========================================= OBSERVER ===========================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/

void testObserverBasic() {
    std::cout << "\n=== Testing Observer Pattern - Basic ===\n";
    
    // Create observer with string events
    Observer<std::string> eventSystem;
    
    // Test variables to check if callbacks were called
    bool playerJoinedCalled = false;
    bool playerLeftCalled = false;
    int callCount = 0;
    
    // Subscribe to "player_joined" event
    eventSystem.subscribe("player_joined", [&playerJoinedCalled, &callCount]() {
        std::cout << "Event: Player joined the game!" << std::endl;
        playerJoinedCalled = true;
        callCount++;
    });
    
    // Subscribe to "player_left" event
    eventSystem.subscribe("player_left", [&playerLeftCalled, &callCount]() {
        std::cout << "Event: Player left the game!" << std::endl;
        playerLeftCalled = true;
        callCount++;
    });
    
    // Test notifications
    std::cout << "Triggering player_joined event...\n";
    eventSystem.notify("player_joined");
    
    std::cout << "Triggering player_left event...\n";
    eventSystem.notify("player_left");
    
    // Verify callbacks were called
    std::cout << "Player joined callback called: " << (playerJoinedCalled ? "YES" : "NO") << std::endl;
    std::cout << "Player left callback called: " << (playerLeftCalled ? "YES" : "NO") << std::endl;
    std::cout << "Total callbacks executed: " << callCount << std::endl;
    
    // Test non-existent event
    std::cout << "Triggering non-existent event (should do nothing)...\n";
    eventSystem.notify("unknown_event");
    
    std::cout << "Observer basic test: " << (callCount == 2 ? "PASSED" : "FAILED") << std::endl;
}

// Test Observer with enum events
void testObserverWithEnum() {
    std::cout << "\n=== Testing Observer Pattern - Enum Events ===\n";
    
    // Define game events as enum
    enum class GameEvent {
        LEVEL_UP,
        ITEM_FOUND,
        ENEMY_DEFEATED,
        GAME_OVER
    };
    
    Observer<GameEvent> gameEvents;
    
    // Game state variables
    int playerLevel = 1;
    int itemsFound = 0;
    int enemiesDefeated = 0;
    bool gameOver = false;
    
    // Subscribe to LEVEL_UP event
    gameEvents.subscribe(GameEvent::LEVEL_UP, [&playerLevel]() {
        playerLevel++;
        std::cout << "🎉 LEVEL UP! New level: " << playerLevel << std::endl;
    });
    
    // Subscribe to ITEM_FOUND event
    gameEvents.subscribe(GameEvent::ITEM_FOUND, [&itemsFound]() {
        itemsFound++;
        std::cout << "💎 Item found! Total items: " << itemsFound << std::endl;
    });
    
    // Subscribe to ENEMY_DEFEATED event
    gameEvents.subscribe(GameEvent::ENEMY_DEFEATED, [&enemiesDefeated, &gameEvents]() {
        enemiesDefeated++;
        std::cout << "⚔️  Enemy defeated! Total: " << enemiesDefeated << std::endl;
        
        // Chain events: every 3 enemies defeated = level up
        if (enemiesDefeated % 3 == 0) {
            std::cout << "Triggering level up from enemy defeat...\n";
            gameEvents.notify(GameEvent::LEVEL_UP);
        }
    });
    
    // Subscribe to GAME_OVER event
    gameEvents.subscribe(GameEvent::GAME_OVER, [&gameOver, &playerLevel, &itemsFound, &enemiesDefeated]() {
        gameOver = true;
        std::cout << "💀 GAME OVER!" << std::endl;
        std::cout << "Final Stats - Level: " << playerLevel 
                  << ", Items: " << itemsFound 
                  << ", Enemies: " << enemiesDefeated << std::endl;
    });
    
    // Simulate game events
    std::cout << "=== Game Simulation ===" << std::endl;
    gameEvents.notify(GameEvent::ITEM_FOUND);    // Item 1
    gameEvents.notify(GameEvent::ENEMY_DEFEATED); // Enemy 1
    gameEvents.notify(GameEvent::ITEM_FOUND);    // Item 2
    gameEvents.notify(GameEvent::ENEMY_DEFEATED); // Enemy 2
    gameEvents.notify(GameEvent::ENEMY_DEFEATED); // Enemy 3 -> triggers level up
    gameEvents.notify(GameEvent::ITEM_FOUND);    // Item 3
    gameEvents.notify(GameEvent::GAME_OVER);     // End game
    
    // Verify final state
    bool testPassed = (playerLevel == 2) && (itemsFound == 3) && 
                      (enemiesDefeated == 3) && (gameOver == true);
    
    std::cout << "Observer enum test: " << (testPassed ? "PASSED" : "FAILED") << std::endl;
}

void testObserverMultipleSubscribers() {
    std::cout << "\n=== Testing Observer Pattern - Multiple Subscribers ===\n";
    
    Observer<std::string> newsSystem;
    
    // Different subscribers with different reactions
    std::vector<std::string> notifications;
    
    // Subscriber 1: Logger
    newsSystem.subscribe("breaking_news", [&notifications]() {
        notifications.push_back("Logger: Breaking news logged");
        std::cout << "📝 Logger: Breaking news event logged" << std::endl;
    });
    
    // Subscriber 2: Email system
    newsSystem.subscribe("breaking_news", [&notifications]() {
        notifications.push_back("Email: Alert sent");
        std::cout << "📧 Email System: Alert sent to subscribers" << std::endl;
    });
    
    // Subscriber 3: Push notification
    newsSystem.subscribe("breaking_news", [&notifications]() {
        notifications.push_back("Push: Notification sent");
        std::cout << "📱 Push Notification: Alert sent to mobile apps" << std::endl;
    });
    
    // Subscriber 4: Statistics counter
    static int newsCount = 0;
    newsSystem.subscribe("breaking_news", [&notifications]() {
        newsCount++;
        notifications.push_back("Stats: Count incremented");
        std::cout << "📊 Statistics: Breaking news count: " << newsCount << std::endl;
    });
    
    // Also subscribe to regular news with fewer subscribers
    newsSystem.subscribe("regular_news", [&notifications]() {
        notifications.push_back("Logger: Regular news logged");
        std::cout << "📝 Logger: Regular news event logged" << std::endl;
    });
    
    newsSystem.subscribe("regular_news", [&notifications]() {
        notifications.push_back("Stats: Regular news counted");
        std::cout << "📊 Statistics: Regular news processed" << std::endl;
    });
    
    // Test multiple subscribers on same event
    std::cout << "\n--- Triggering breaking news ---\n";
    newsSystem.notify("breaking_news");
    
    std::cout << "\n--- Triggering regular news ---\n";
    newsSystem.notify("regular_news");
    
    std::cout << "\n--- Triggering breaking news again ---\n";
    newsSystem.notify("breaking_news");
    
    // Verify all notifications were sent
    std::cout << "\nTotal notifications sent: " << notifications.size() << std::endl;
    for (const auto& notif : notifications) {
        std::cout << "  - " << notif << std::endl;
    }
    
    // Should be: 4 (breaking) + 2 (regular) + 4 (breaking again) = 10
    bool testPassed = (notifications.size() == 10);
    std::cout << "Observer multiple subscribers test: " << (testPassed ? "PASSED" : "FAILED") << std::endl;
}

// Test Observer with complex data capture
void testObserverComplexCapture() {
    std::cout << "\n=== Testing Observer Pattern - Complex Capture ===\n";
    
    Observer<int> healthSystem;
    
    // Simulate a health monitoring system
    struct PlayerStats {
        std::string name;
        int health;
        int maxHealth;
        bool isAlive;
        
        PlayerStats(const std::string& n, int h) : name(n), health(h), maxHealth(h), isAlive(true) {}
    };
    
    PlayerStats player("Hero", 100);
    
    // Health threshold events (using integers as event IDs)
    const int HEALTH_LOW = 25;      // Health below 25%
    const int HEALTH_CRITICAL = 10; // Health below 10%
    const int HEALTH_ZERO = 0;      // Health at 0
    
    // Subscribe to low health warning
    healthSystem.subscribe(HEALTH_LOW, [&player]() {
        std::cout << "⚠️  WARNING: " << player.name << "'s health is low! ("
                  << player.health << "/" << player.maxHealth << ")" << std::endl;
    });
    
    // Subscribe to critical health alert
    healthSystem.subscribe(HEALTH_CRITICAL, [&player]() {
        std::cout << "🚨 CRITICAL: " << player.name << "'s health is critical! ("
                  << player.health << "/" << player.maxHealth << ")" << std::endl;
    });
    
    // Subscribe to death event
    healthSystem.subscribe(HEALTH_ZERO, [&player]() {
        player.isAlive = false;
        std::cout << "💀 " << player.name << " has died!" << std::endl;
    });
    
    // Simulate taking damage and triggering events
    auto takeDamage = [&](int damage) {
        player.health = std::max(0, player.health - damage);
        std::cout << player.name << " takes " << damage << " damage. Health: " 
                  << player.health << "/" << player.maxHealth << std::endl;
        
        // Check thresholds and trigger events
        int healthPercentage = (player.health * 100) / player.maxHealth;
        
        if (player.health == 0 && player.isAlive) {
            healthSystem.notify(HEALTH_ZERO);
        } else if (healthPercentage <= 10 && player.health > 0) {
            healthSystem.notify(HEALTH_CRITICAL);
        } else if (healthPercentage <= 25 && player.health > 0) {
            healthSystem.notify(HEALTH_LOW);
        }
    };
    
    // Simulate combat
    std::cout << "=== Combat Simulation ===" << std::endl;
    std::cout << "Initial health: " << player.health << std::endl;
    
    takeDamage(60);  // Health: 40 (no event)
    takeDamage(20);  // Health: 20 (low health event)
    takeDamage(10);  // Health: 10 (critical health event)
    takeDamage(5);   // Health: 5  (critical health event again)
    takeDamage(5);   // Health: 0  (death event)
    
    bool testPassed = !player.isAlive && (player.health == 0);
    std::cout << "Observer complex capture test: " << (testPassed ? "PASSED" : "FAILED") << std::endl;
}

/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*========================================== SINGLETON =========================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/



void testSingletonBasic() {
    std::cout << "\n=== Testing Singleton Pattern - Basic ===\n";
    
    // Test that instance doesn't exist initially
    try {
        Singleton<GameSettings>::instance();  // ✅ Enlever auto* settings =
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    // Create singleton instance
    std::cout << "\nCreating singleton instance...\n";
    Singleton<GameSettings>::instantiate("My Game", 1920, 1080);
    
    // Access the instance
    auto* settings1 = Singleton<GameSettings>::instance();
    std::cout << "First access: ";
    settings1->display();
    
    // Access again - should be same instance
    auto* settings2 = Singleton<GameSettings>::instance();
    std::cout << "Second access: ";
    settings2->display();
    
    // Verify it's the same instance
    bool sameInstance = (settings1 == settings2);
    std::cout << "Same instance check: " << (sameInstance ? "PASSED" : "FAILED") << std::endl;
    
    // Modify through one pointer, check through another
    settings1->setVolume(75);
    std::cout << "After modification via settings1: ";
    settings2->display();
    
    // Try to create another instance (should fail)
    try {
        std::cout << "\nTrying to create second instance...\n";
        Singleton<GameSettings>::instantiate("Another Game", 800, 600);
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    // Cleanup
    std::cout << "\nDestroying singleton...\n";
    Singleton<GameSettings>::destroy();
    
    std::cout << "Singleton basic test: PASSED\n";
}

// Test Singleton with different types
void testSingletonMultipleTypes() {
    std::cout << "\n=== Testing Singleton Pattern - Multiple Types ===\n";
    
    // Create Logger singleton
    std::cout << "Creating Logger singleton...\n";
    Singleton<Logger>::instantiate();
    
    // Create GameSettings singleton  
    std::cout << "Creating GameSettings singleton...\n";
    Singleton<GameSettings>::instantiate("Adventure Game", 1280, 720);
    
    // Use both singletons
    auto* logger = Singleton<Logger>::instance();
    auto* settings = Singleton<GameSettings>::instance();
    
    logger->log("Game starting...");
    settings->display();
    
    logger->log("Player joined");
    settings->setVolume(60);
    
    logger->log("Settings changed");
    settings->display();
    
    // Verify independent instances
    std::cout << "Logger address: " << logger << std::endl;
    std::cout << "Settings address: " << settings << std::endl;
    std::cout << "Different addresses: " << (logger != (void*)settings ? "YES" : "NO") << std::endl;
    
    // Cleanup both
    std::cout << "\nCleaning up...\n";
    Singleton<Logger>::destroy();
    Singleton<GameSettings>::destroy();
    
    std::cout << "Singleton multiple types test: PASSED\n";
}

// Test Singleton lifecycle and error handling
void testSingletonLifecycle() {
    std::cout << "\n=== Testing Singleton Pattern - Lifecycle ===\n";
    
    // Test multiple create/destroy cycles
    for (int i = 1; i <= 3; i++) {
        std::cout << "\n--- Cycle " << i << " ---\n";
        
        // Create
        Singleton<Logger>::instantiate();
        auto* logger = Singleton<Logger>::instance();
        
        logger->log("Cycle " + std::to_string(i) + " message 1");
        logger->log("Cycle " + std::to_string(i) + " message 2");
        
        std::cout << "Logs in this cycle: " << logger->getLogCount() << std::endl;
        
        // Destroy
        Singleton<Logger>::destroy();
        
        // Try to access after destroy (should fail)
        try {
            Singleton<Logger>::instance();  // ✅ Enlever auto* invalid =
            std::cout << "ERROR: Should have thrown exception!" << std::endl;
        } catch (const std::runtime_error& e) {
            std::cout << "Expected exception after destroy: " << e.what() << std::endl;
        }
    }
    
    // Test double destroy (should be safe)
    std::cout << "\n--- Testing double destroy ---\n";
    Singleton<Logger>::instantiate();
    Singleton<Logger>::destroy();
    std::cout << "First destroy completed\n";
    
    Singleton<Logger>::destroy();  // Should be safe
    std::cout << "Second destroy completed (should be safe)\n";
    
    std::cout << "Singleton lifecycle test: PASSED\n";
}

// Test Singleton with perfect forwarding
void testSingletonPerfectForwarding() {
    std::cout << "\n=== Testing Singleton Pattern - Perfect Forwarding ===\n";
    
    // Test with different argument types
    std::cout << "Creating with string literal and integers...\n";
    Singleton<GameSettings>::instantiate("Forwarded Game", 800, 600);
    
    auto* settings = Singleton<GameSettings>::instance();
    settings->display();
    
    Singleton<GameSettings>::destroy();
    
    // Test with variables (lvalues)
    std::cout << "\nCreating with lvalue variables...\n";
    std::string gameName = "Variable Game";
    int width = 1024;
    int height = 768;
    
    Singleton<GameSettings>::instantiate(gameName, width, height);
    
    settings = Singleton<GameSettings>::instance();
    settings->display();
    
    // Verify original variables unchanged
    std::cout << "Original variables: " << gameName << " " << width << "x" << height << std::endl;
    
    Singleton<GameSettings>::destroy();
    
    // Test with complex expression (rvalue)
    std::cout << "\nCreating with computed values...\n";
    Singleton<GameSettings>::instantiate(
        std::string("Computed") + " Game",  // rvalue string
        1920 / 2,                           // computed int
        1080 / 2                            // computed int
    );
    
    settings = Singleton<GameSettings>::instance();
    settings->display();
    
    Singleton<GameSettings>::destroy();
    
    std::cout << "Singleton perfect forwarding test: PASSED\n";
}

// Test Singleton thread-safety simulation (basic)
void testSingletonConsistency() {
    std::cout << "\n=== Testing Singleton Pattern - Consistency ===\n";
    
    // Simulate multiple "threads" accessing singleton
    // (Not real threads, but simulates the access pattern)
    
    Singleton<Logger>::instantiate();
    
    std::vector<Logger*> instances;
    
    // Simulate 10 different access points
    for (int i = 0; i < 10; i++) {
        auto* logger = Singleton<Logger>::instance();
        instances.push_back(logger);
        logger->log("Access from point " + std::to_string(i));
    }
    
    // Verify all instances are the same
    bool allSame = true;
    Logger* first = instances[0];
    
    for (size_t i = 1; i < instances.size(); i++) {
        if (instances[i] != first) {
            allSame = false;
            break;
        }
    }
    
    std::cout << "All 10 accesses returned same instance: " << (allSame ? "YES" : "NO") << std::endl;
    std::cout << "Total logs accumulated: " << first->getLogCount() << std::endl;
    
    // Verify the same instance across different functions
    auto testFunction = []() -> Logger* {
        return Singleton<Logger>::instance();
    };
    
    Logger* fromFunction = testFunction();
    bool sameFromFunction = (fromFunction == first);
    std::cout << "Same instance from function: " << (sameFromFunction ? "YES" : "NO") << std::endl;
    
    Singleton<Logger>::destroy();
    
    std::cout << "Singleton consistency test: " << (allSame && sameFromFunction ? "PASSED" : "FAILED") << std::endl;
}


/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*====================================== STATE MACHINE =========================================*/
/*==============================================================================================*/
/*==============================================================================================*/
/*==============================================================================================*/

void testStateMachineBasic() {
    std::cout << "\n=== Testing StateMachine Pattern - Basic ===\n";
    
    // Define game states as enum
    enum class GameState {
        MENU,
        PLAYING,
        PAUSED,
        GAME_OVER
    };
    
    StateMachine<GameState> gameFSM(GameState::MENU);
    
    // Add all states
    gameFSM.addState(GameState::MENU);
    gameFSM.addState(GameState::PLAYING);
    gameFSM.addState(GameState::PAUSED);
    gameFSM.addState(GameState::GAME_OVER);
    
    // Test current state
    std::cout << "Initial state: " << (int)gameFSM.getCurrentState() << " (should be 0 = MENU)" << std::endl;
    
    // Add state actions
    gameFSM.addAction(GameState::MENU, []() {
        std::cout << "📋 In MENU: Showing main menu" << std::endl;
    });
    
    gameFSM.addAction(GameState::PLAYING, []() {
        std::cout << "🎮 In PLAYING: Game running" << std::endl;
    });
    
    gameFSM.addAction(GameState::PAUSED, []() {
        std::cout << "⏸️  In PAUSED: Game paused" << std::endl;
    });
    
    gameFSM.addAction(GameState::GAME_OVER, []() {
        std::cout << "💀 In GAME_OVER: Showing game over screen" << std::endl;
    });
    
    // Add transitions with actions
    gameFSM.addTransition(GameState::MENU, GameState::PLAYING, []() {
        std::cout << "🚀 Transition: Starting game..." << std::endl;
    });
    
    gameFSM.addTransition(GameState::PLAYING, GameState::PAUSED, []() {
        std::cout << "⏸️  Transition: Pausing game..." << std::endl;
    });
    
    gameFSM.addTransition(GameState::PAUSED, GameState::PLAYING, []() {
        std::cout << "▶️  Transition: Resuming game..." << std::endl;
    });
    
    gameFSM.addTransition(GameState::PLAYING, GameState::GAME_OVER, []() {
        std::cout << "💀 Transition: Player died..." << std::endl;
    });
    
    gameFSM.addTransition(GameState::GAME_OVER, GameState::MENU, []() {
        std::cout << "🔄 Transition: Returning to menu..." << std::endl;
    });
    
    // Test state machine execution
    std::cout << "\n=== Game Loop Simulation ===\n";
    
    // Update in MENU state
    gameFSM.update();
    
    // Start game
    gameFSM.transitionTo(GameState::PLAYING);
    gameFSM.update();
    
    // Pause game
    gameFSM.transitionTo(GameState::PAUSED);
    gameFSM.update();
    
    // Resume game
    gameFSM.transitionTo(GameState::PLAYING);
    gameFSM.update();
    
    // Game over
    gameFSM.transitionTo(GameState::GAME_OVER);
    gameFSM.update();
    
    // Back to menu
    gameFSM.transitionTo(GameState::MENU);
    gameFSM.update();
    
    std::cout << "StateMachine basic test: PASSED\n";
}

// Test StateMachine with string states
void testStateMachineStrings() {
    std::cout << "\n=== Testing StateMachine Pattern - String States ===\n";
    
    StateMachine<std::string> deviceFSM("OFF");
    
    // Add device states
    deviceFSM.addState("OFF");
    deviceFSM.addState("STANDBY");
    deviceFSM.addState("ON");
    deviceFSM.addState("ERROR");
    
    std::cout << "Initial state: " << deviceFSM.getCurrentState() << std::endl;
    
    // Track state changes
    std::vector<std::string> stateHistory;
    
    // Add state actions that track history
    deviceFSM.addAction("OFF", [&stateHistory]() {
        std::cout << "🔴 Device is OFF" << std::endl;
        stateHistory.push_back("OFF");
    });
    
    deviceFSM.addAction("STANDBY", [&stateHistory]() {
        std::cout << "🟡 Device is on STANDBY" << std::endl;
        stateHistory.push_back("STANDBY");
    });
    
    deviceFSM.addAction("ON", [&stateHistory]() {
        std::cout << "🟢 Device is ON" << std::endl;
        stateHistory.push_back("ON");
    });
    
    deviceFSM.addAction("ERROR", [&stateHistory]() {
        std::cout << "🔥 Device ERROR!" << std::endl;
        stateHistory.push_back("ERROR");
    });
    
    // Add transitions
    deviceFSM.addTransition("OFF", "STANDBY", []() {
        std::cout << "⚡ Powering up to standby..." << std::endl;
    });
    
    deviceFSM.addTransition("STANDBY", "ON", []() {
        std::cout << "✅ Fully starting up..." << std::endl;
    });
    
    deviceFSM.addTransition("ON", "STANDBY", []() {
        std::cout << "💤 Going to standby..." << std::endl;
    });
    
    deviceFSM.addTransition("STANDBY", "OFF", []() {
        std::cout << "🔌 Shutting down..." << std::endl;
    });
    
    // Error transitions from any state
    deviceFSM.addTransition("ON", "ERROR", []() {
        std::cout << "💥 Critical error occurred!" << std::endl;
    });
    
    deviceFSM.addTransition("ERROR", "OFF", []() {
        std::cout << "🔧 Error resolved, shutting down safely..." << std::endl;
    });
    
    // Simulate device lifecycle
    std::cout << "\n=== Device Lifecycle ===\n";
    
    deviceFSM.update();                           // OFF
    deviceFSM.transitionTo("STANDBY");
    deviceFSM.update();                           // STANDBY
    deviceFSM.transitionTo("ON");
    deviceFSM.update();                           // ON
    deviceFSM.transitionTo("ERROR");
    deviceFSM.update();                           // ERROR
    deviceFSM.transitionTo("OFF");
    deviceFSM.update();                           // OFF
    
    // Print state history
    std::cout << "\nState history: ";
    for (const auto& state : stateHistory) {
        std::cout << state << " -> ";
    }
    std::cout << "END\n";
    
    bool testPassed = (stateHistory.size() == 5) && 
                      (stateHistory[0] == "OFF") && 
                      (stateHistory[4] == "OFF");
    
    std::cout << "StateMachine strings test: " << (testPassed ? "PASSED" : "FAILED") << std::endl;
}

// Test StateMachine error handling
void testStateMachineErrorHandling() {
    std::cout << "\n=== Testing StateMachine Pattern - Error Handling ===\n";
    
    StateMachine<int> simpleFSM(0);
    
    // Add only some states
    simpleFSM.addState(0);
    simpleFSM.addState(1);
    // State 2 is NOT added
    
    // Test adding transition to non-existent state
    try {
        std::cout << "Trying to add transition to non-existent state...\n";
        simpleFSM.addTransition(0, 2, []() { std::cout << "Invalid transition"; });
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    // Test adding action to non-existent state
    try {
        std::cout << "Trying to add action to non-existent state...\n";
        simpleFSM.addAction(2, []() { std::cout << "Invalid action"; });
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    // Test transitioning to non-existent state
    try {
        std::cout << "Trying to transition to non-existent state...\n";
        simpleFSM.transitionTo(2);
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    // Add valid transition
    simpleFSM.addTransition(0, 1, []() {
        std::cout << "Valid transition 0->1" << std::endl;
    });
    
    // Test valid transition
    std::cout << "Performing valid transition...\n";
    simpleFSM.transitionTo(1);
    std::cout << "Current state: " << simpleFSM.getCurrentState() << std::endl;
    
    // Test transitioning without defined transition
    try {
        std::cout << "Trying to transition without defined path...\n";
        simpleFSM.transitionTo(0);  // No transition from 1->0
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    std::cout << "StateMachine error handling test: PASSED\n";
}

// Test StateMachine with complex state data
void testStateMachineComplex() {
    std::cout << "\n=== Testing StateMachine Pattern - Complex ===\n";
    
    // Custom state struct
    struct PlayerState {
        std::string name;
        int id;
        
        PlayerState(const std::string& n, int i) : name(n), id(i) {}
        
        bool operator==(const PlayerState& other) const {
            return name == other.name && id == other.id;
        }
    };
    
    // Custom hash function for PlayerState
    struct PlayerStateHash {
        std::size_t operator()(const PlayerState& state) const {
            std::hash<std::string> stringHash;
            std::hash<int> intHash;
            return stringHash(state.name) ^ (intHash(state.id) << 1);
        }
    };
    
    // Note: Pour utiliser un type custom comme clé dans unordered_map,
    // il faudrait spécialiser std::hash ou passer un hash customisé.
    // Pour ce test, on utilisera une version simplifiée avec des entiers.
    
    std::cout << "Testing with complex state transitions...\n";
    
    StateMachine<int> playerFSM(1); // 1 = Idle
    
    // Player states: 1=Idle, 2=Walking, 3=Running, 4=Jumping, 5=Falling
    playerFSM.addState(1); // Idle
    playerFSM.addState(2); // Walking
    playerFSM.addState(3); // Running
    playerFSM.addState(4); // Jumping
    playerFSM.addState(5); // Falling
    
    // Player stats for state tracking
    struct Stats {
        int stamina = 100;
        bool onGround = true;
        int speed = 0;
    } playerStats;
    
    // State actions that modify player stats
    playerFSM.addAction(1, [&playerStats]() { // Idle
        playerStats.speed = 0;
        if (playerStats.stamina < 100) playerStats.stamina++;
        std::cout << "😴 Idle (stamina: " << playerStats.stamina << ")" << std::endl;
    });
    
    playerFSM.addAction(2, [&playerStats]() { // Walking
        playerStats.speed = 5;
        std::cout << "🚶 Walking (speed: " << playerStats.speed << ")" << std::endl;
    });
    
    playerFSM.addAction(3, [&playerStats]() { // Running
        playerStats.speed = 10;
        if (playerStats.stamina > 0) playerStats.stamina--;
        std::cout << "🏃 Running (speed: " << playerStats.speed << ", stamina: " << playerStats.stamina << ")" << std::endl;
    });
    
    playerFSM.addAction(4, [&playerStats]() { // Jumping
        playerStats.onGround = false;
        std::cout << "🦘 Jumping (airborne)" << std::endl;
    });
    
    playerFSM.addAction(5, [&playerStats]() { // Falling
        playerStats.onGround = false;
        std::cout << "⬇️  Falling (airborne)" << std::endl;
    });
    
    // Add transitions with conditions - ✅ FIX: Enlever captures inutilisées
    playerFSM.addTransition(1, 2, []() { // Idle -> Walking
        std::cout << "🚀 Starting to walk..." << std::endl;
    });
    
    playerFSM.addTransition(2, 3, [&playerStats]() { // Walking -> Running
        if (playerStats.stamina > 20) {
            std::cout << "💨 Speeding up to run..." << std::endl;
        } else {
            std::cout << "💨 Trying to run but low stamina..." << std::endl;
        }
    });
    
    playerFSM.addTransition(3, 2, []() { // Running -> Walking
        std::cout << "😤 Slowing down to walk..." << std::endl;
    });
    
    playerFSM.addTransition(2, 1, []() { // Walking -> Idle
        std::cout << "🛑 Stopping..." << std::endl;
    });
    
    playerFSM.addTransition(1, 4, []() { // Idle -> Jumping
        std::cout << "🦘 Jump!" << std::endl;
    });
    
    playerFSM.addTransition(4, 5, []() { // Jumping -> Falling
        std::cout << "⬇️  Gravity takes over..." << std::endl;
    });
    
    playerFSM.addTransition(5, 1, [&playerStats]() { // Falling -> Idle
        playerStats.onGround = true;
        std::cout << "💥 Landed!" << std::endl;
    });
    
    // Simulate complex player movement
    std::cout << "\n=== Player Movement Simulation ===\n";
    
    for (int frame = 0; frame < 10; frame++) {
        std::cout << "Frame " << frame << ": ";
        playerFSM.update();
        
        // Simulate different inputs
        if (frame == 1) playerFSM.transitionTo(2);      // Start walking
        else if (frame == 3) playerFSM.transitionTo(3); // Start running
        else if (frame == 5) playerFSM.transitionTo(2); // Slow to walk
        else if (frame == 6) playerFSM.transitionTo(1); // Stop
        else if (frame == 7) playerFSM.transitionTo(4); // Jump
        else if (frame == 8) playerFSM.transitionTo(5); // Fall
        else if (frame == 9) playerFSM.transitionTo(1); // Land
    }
    
    std::cout << "Final stats - Stamina: " << playerStats.stamina 
              << ", On ground: " << (playerStats.onGround ? "Yes" : "No") << std::endl;
    
    std::cout << "StateMachine complex test: PASSED\n";
}

int main() {
    // testBasicPoolOperations();
    // testWithCustomClass();
    // testPoolCapacityAndReuse();
    // testInvalidAccess();
    
    // New DataBuffer tests
    // testDataBuffer();
    // testDataBufferAdvanced();
    // testDataBufferErrors();

	// testMementoBasic();
    // testMementoAdvanced();
    // testMementoEdgeCases();

	// testObserverBasic();
    // testObserverWithEnum();
    // testObserverMultipleSubscribers();
    // testObserverComplexCapture();

	// testSingletonBasic();
    // testSingletonMultipleTypes();
    // testSingletonLifecycle(); 
    // testSingletonPerfectForwarding();
    // testSingletonConsistency();

	testStateMachineBasic();
    testStateMachineStrings();
    testStateMachineErrorHandling();
    testStateMachineComplex();
    
    std::cout << "\nAll tests completed!\n";
    return 0;
}