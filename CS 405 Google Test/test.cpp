// Uncomment the next line to use precompiled headers
#include "pch.h"
// uncomment the next line if you do not use precompiled headers
//#include "gtest/gtest.h"
//
// the global test environment setup and tear down
// you should not need to change anything here
class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override
    {
        //  initialize random seed
        srand(time(nullptr));
    }

    // Override this to define how to tear down the environment.
    void TearDown() override {}
};

// create our test class to house shared data between tests
// you should not need to change anything here
class CollectionTest : public ::testing::Test
{
protected:
    // create a smart point to hold our collection
    std::unique_ptr<std::vector<int>> collection;

    void SetUp() override
    { // create a new collection to be used in the test
        collection.reset(new std::vector<int>);
    }

    void TearDown() override
    { //  erase all elements in the collection, if any remain
        collection->clear();
        // free the pointer
        collection.reset(nullptr);
    }

    // helper function to add random values from 0 to 99 count times to the collection
    void add_entries(int count)
    {
        if (count <= 0)
            throw std::out_of_range("Cannot add less than 1 entry");
        for (auto i = 0; i < count; ++i)
            collection->push_back(rand() % 100);
    }
};

// When should you use the EXPECT_xxx or ASSERT_xxx macros?
// Use ASSERT when failure should terminate processing, such as the reason for the test case.
// Use EXPECT when failure should notify, but processing should continue

// Test that a collection is empty when created.
// Prior to calling this (and all other TEST_F defined methods),
//  CollectionTest::StartUp is called.
// Following this method (and all other TEST_F defined methods),
//  CollectionTest::TearDown is called
TEST_F(CollectionTest, CollectionSmartPointerIsNotNull)
{
    // is the collection created
    ASSERT_TRUE(collection);

    // if empty, the size must be 0
    ASSERT_NE(collection.get(), nullptr);
}

// Test that a collection is empty when created.
TEST_F(CollectionTest, IsEmptyOnCreate)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // if empty, the size must be 0
    ASSERT_EQ(collection->size(), 0);
}

/* Comment this test out to prevent the test from running
 * Uncomment this test to see a failure in the test explorer */
TEST_F(CollectionTest, AlwaysFail)
{
    FAIL();
}

// DONE: Create a test to verify adding a single value to an empty collection
TEST_F(CollectionTest, CanAddToEmptyVector)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Function to test
    add_entries(1);

    // is the collection still empty?
    EXPECT_FALSE(collection->empty());

    // is the size now 1?
    ASSERT_EQ(collection->size(), 1);
}

// DONE: Create a test to verify adding five values to collection
TEST_F(CollectionTest, CanAddFiveValuesToVector)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    add_entries(5);

    // is the collection still empty?
    EXPECT_FALSE(collection->empty());

    // is the size now five?
    ASSERT_EQ(collection->size(), 5);

}

// DONE: Create a test to verify that max size is greater than or equal to size for 0, 1, 5, 10 entries
TEST_F(CollectionTest, CollectionSizeIsLargeEnough)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // is the vector max size larger than the size for 0 entries?
    ASSERT_GE(collection->max_size(), collection->size());

    // Add an entry and asserts that the size is changed
    add_entries(1);
    EXPECT_EQ(collection->size(), 1);

    // is the vector max size larger than the size for 1 entries?
    ASSERT_GE(collection->max_size(), collection->size());

    // Adds 4 entries, for a total of 5, asserts new size
    add_entries(4);
    EXPECT_EQ(collection->size(), 5);

    // is the vector max size larger than the size for 5 entries?
    ASSERT_GE(collection->max_size(), collection->size());

    // Adds 5 entries for a total of 10, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 10);

    // is the vector max size larger than the size for 10 entries?
    ASSERT_GE(collection->max_size(), collection->size());

}

// DONE: Create a test to verify that capacity is greater than or equal to size for 0, 1, 5, 10 entries
TEST_F(CollectionTest, CollectionCapacityIsGreaterThanSize)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // is the vector capacity >= size for 0 entries?
    ASSERT_GE(collection->capacity(), collection->size());

    // Add an entry and asserts that the size is changed
    add_entries(1);
    EXPECT_EQ(collection->size(), 1);

    // is the vector capacity >= size for 1 entry?
    ASSERT_GE(collection->capacity(), collection->size());

    // Adds 4 entries, for a total of 5, asserts new size
    add_entries(4);
    EXPECT_EQ(collection->size(), 5);

    // is the vector capacity >= size for 5 entries?
    ASSERT_GE(collection->capacity(), collection->size());

    // Adds 5 entries for a total of 10, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 10);

    // is the vector capacity >= size for 10 entries?
    ASSERT_GE(collection->capacity(), collection->size());
}

// DONE: Create a test to verify resizing increases the collection
TEST_F(CollectionTest, CanResizeIncreaseCollectionSize)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // resizes the vector to 5 elements
    collection->resize(5);

    // is the collection empty?
    EXPECT_FALSE(collection->empty());

    // is the collection's size now 5?
    ASSERT_EQ(collection->size(), 5);
}

// DONE: Create a test to verify resizing decreases the collection
TEST_F(CollectionTest, CanResizeDecreaseCollectionSize)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Adds 5 entries, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 5);

    // resizes vector to 1 element
    collection->resize(1);

    // Has vector size changed?
    EXPECT_NE(collection->size(), 5);

    // Is the new size 1?
    ASSERT_EQ(collection->size(), 1);

}

// DONE: Create a test to verify resizing decreases the collection to zero
TEST_F(CollectionTest, CanResizeEmptyCollection)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Adds 5 entries, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 5);

    // resizes vector to 0 elements
    collection->resize(0);

    // Has vector size changed?
    EXPECT_NE(collection->size(), 5);

    // Is the new size 0?
    ASSERT_EQ(collection->size(), 0);
}

// DONE: Create a test to verify clear erases the collection
TEST_F(CollectionTest, DoesClearEmptyCollection)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Adds 5 entries, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 5);

    // clears vector
    collection->clear();

    // Has vector size changed?
    EXPECT_NE(collection->size(), 5);

    // Is the new size 0?
    ASSERT_EQ(collection->size(), 0);
}

// DONE: Create a test to verify erase(begin,end) erases the collection
TEST_F(CollectionTest, DoesEraseEmptyCollection)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Adds 5 entries, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 5);

    // erases vector using iterator
    collection->erase(collection->begin(), collection->end());

    // Has vector size changed?
    EXPECT_NE(collection->size(), 5);

    // Is the new size 0?
    ASSERT_EQ(collection->size(), 0);
}

// DONE: Create a test to verify reserve increases the capacity but not the size of the collection
TEST_F(CollectionTest, DoesReserveIncreaseCapacityNotSize)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // is the capacity 0?
    EXPECT_EQ(collection->capacity(), 0);

    // is the capacity equal to the size?

    EXPECT_EQ(collection->capacity(), collection->size());

    // Reserves 5 elements of memeory
    collection->reserve(5);

    // is capacity still 0?
    EXPECT_NE(collection->capacity(), 0);

    // is the capacity still equal to the size?
    EXPECT_NE(collection->capacity(), collection->size());

    // is the capacity now 5?
    ASSERT_EQ(collection->capacity(), 5);
}

// DONE: Create a test to verify the std::out_of_range exception is thrown when calling at() with an index out of bounds
// NOTE: This is a negative test
TEST_F(CollectionTest, ExceptionThrownWhenCallingOutOfVectorRange)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Adds 5 entries, asserts new size
    add_entries(5);
    EXPECT_EQ(collection->size(), 5);

    // Does calling an out of range index throw the out of range exception?
    ASSERT_THROW(int i = collection->at(6), std::out_of_range);
}

// TODO: Create 2 unit tests of your own to test something on the collection - do 1 positive & 1 negative

// A test that ensures values added with add_entries() are within bounds 0 - 99
TEST_F(CollectionTest, AddEntriesValuesStayInRange)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // Adds 5 entries, asserts new size
    add_entries(1000);
    ASSERT_EQ(collection->size(), 1000);

    // For all newly added values
    for (int i = 0; i < collection->size(); i++)
    {
        // is the value >=0?
        EXPECT_GE(collection->at(i), 0);

        // is the value < 100?
        EXPECT_LT(collection->at(i), 100);
    }
}


// A test that 
// NOTE : This is a negative test
TEST_F(CollectionTest, CannotAddLessThanOneEntry)
{
    // Can I add 0 entries?
    EXPECT_THROW(add_entries(0), std::out_of_range);

    // Can I add a negative number of entries?
    ASSERT_THROW(add_entries(-1), std::out_of_range);

}