#include <cxxtest/TestSuite.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/cogserver/server/CogServer.h>
#include <opencog/attentionbank/types/atom_types.h>
#include <opencog/attentionbank/bank/AttentionBank.h>
#include <opencog/util/Logger.h>
#include <opencog/cogserver/modules/agents/Agent.h>
#include "RentCollectionBaseAgent.h"

class RentCollectionBaseAgentTest : public CxxTest::TestSuite {
private:
    CogServer* cogserver;
    AtomSpace* atomSpace;
    RentCollectionBaseAgent* rentCollectionAgent;
    AttentionBank* bank;

public:
    void setUp() override {
        cogserver = &cogserver();
        cogserver->loadModules();  // Ensure all necessary modules are loaded
        atomSpace = &cogserver->getAtomSpace();
        rentCollectionAgent = new RentCollectionBaseAgent(*cogserver);  // Initialize RentCollectionBaseAgent
        bank = &cogserver->getAttentionBank();
    }

    void tearDown() override {
        delete rentCollectionAgent;  // Clean up RentCollectionBaseAgent
    }

    // Test case for verifying that the agent calculates the correct STI Rent
    void testCalculateSTIRent() {
        // Simulate some conditions for STI Funds
        bank->setSTIFunds(500);  // Set initial STI funds
        rentCollectionAgent->load_params();  // Load agent parameters

        // Calculate STI Rent based on current parameters and funds
        AttentionValue::sti_t rent = rentCollectionAgent->calculate_STI_Rent();

        // Assert that the rent is calculated correctly based on the set conditions
        TS_ASSERT_DIFFERS(rent, 0);  // Ensure that rent is non-zero (assuming the funds are not exactly at the target)
    }

    // Test case for verifying that the agent calculates the correct LTI Rent
    void testCalculateLTIRent() {
        // Simulate some conditions for LTI Funds
        bank->setLTIFunds(300);  // Set initial LTI funds
        rentCollectionAgent->load_params();  // Load agent parameters

        // Calculate LTI Rent based on current parameters and funds
        AttentionValue::sti_t rent = rentCollectionAgent->calculate_LTI_Rent();

        // Assert that the rent is calculated correctly based on the set conditions
        TS_ASSERT_DIFFERS(rent, 0);  // Ensure that rent is non-zero (assuming the funds are not exactly at the target)
    }

    // Test case to check that the rent collection logic works as expected
    void testCollectRent() {
        // Simulate conditions and parameters
        rentCollectionAgent->load_params();  // Load agent parameters

        // Assuming `collectRent` modifies the bank's funds, we will track funds before and after
        double initialSTIFunds = bank->getSTIFunds();
        double initialLTIFunds = bank->getLTIFunds();

        // Run the RentCollectionAgent's run method (which will invoke collectRent)
        rentCollectionAgent->run();

        // Ensure that the funds are adjusted (this assumes the agent will change the funds)
        TS_ASSERT_DIFFERS(bank->getSTIFunds(), initialSTIFunds);  // STI Funds should change
        TS_ASSERT_DIFFERS(bank->getLTIFunds(), initialLTIFunds);  // LTI Funds should change
    }

    // Test case to ensure that parameters are loaded correctly
    void testLoadParams() {
        // Initially check if the parameters are loaded correctly
        rentCollectionAgent->load_params();

        // Here, you could assert on the individual parameters if they are set as expected
        TS_ASSERT_EQUALS(rentCollectionAgent->calculate_STI_Rent(), rentCollectionAgent->STIAtomRent);  // Check STI rent initialization
        TS_ASSERT_EQUALS(rentCollectionAgent->calculate_LTI_Rent(), rentCollectionAgent->LTIAtomRent);  // Check LTI rent initialization
    }
};
