#include <cxxtest/TestSuite.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/cogserver/server/CogServer.h>
#include <opencog/attentionbank/types/atom_types.h>
#include <opencog/atomspace/Atom.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/attentionbank/bank/AttentionBank.h>
#include <opencog/util/Logger.h>
#include <opencog/cogserver/modules/agents/Agent.h>
#include "ForgettingAgent.h"

class ForgettingAgentTest : public CxxTest::TestSuite {
private:
    CogServer* cogserver;
    AtomSpace* atomSpace;
    ForgettingAgent* forgettingAgent;

public:
    // Set up the environment before each test
    void setUp() override {
        cogserver = &cogserver();
        cogserver->loadModules();  // Ensure all necessary modules are loaded
        atomSpace = &cogserver->getAtomSpace();
        forgettingAgent = new ForgettingAgent(*cogserver);  // Initialize the ForgettingAgent
    }

    // Clean up the environment after each test
    void tearDown() override {
        delete forgettingAgent;  // Clean up ForgettingAgent to free resources
    }

    // Test case for forgetting atoms based on their truth values
    void testForgetAtoms() {
        // Create atoms and assign truth values
        Handle atom1 = atomSpace->add_node(CONCEPT_NODE, "Atom1");
        atom1->setTruthValue(SimpleTruthValue::createTV(0.5, 0.9));
        Handle atom2 = atomSpace->add_node(CONCEPT_NODE, "Atom2");
        atom2->setTruthValue(SimpleTruthValue::createTV(0.3, 0.8));
        Handle atom3 = atomSpace->add_node(CONCEPT_NODE, "Atom3");
        atom3->setTruthValue(SimpleTruthValue::createTV(0.2, 0.7));

        // Create links between atoms
        Handle link = atomSpace->add_link(INHERITANCE_LINK, atom1, atom2);
        link->setTruthValue(SimpleTruthValue::createTV(0.8, 0.9));
        link = atomSpace->add_link(INHERITANCE_LINK, atom2, atom3);
        link->setTruthValue(SimpleTruthValue::createTV(0.7, 0.9));

        // Set ForgettingAgent parameters
        forgettingAgent->maxSize = 2;  // Maximum size of AtomSpace
        forgettingAgent->forgetThreshold = 0.3;  // Forget threshold for atoms

        // Run the ForgettingAgent to process atoms
        forgettingAgent->run();

        // After execution, atom2 and atom3 should be forgotten, leaving only atom1
        HandleSeq remainingAtoms;
        atomSpace->get_handles_by_type(std::back_inserter(remainingAtoms), ATOM, true);

        // Assert that only atom1 remains in AtomSpace
        TS_ASSERT_EQUALS(remainingAtoms.size(), 1);  // Only atom1 should remain
    }

    // Test case where no atoms are forgotten because the size of AtomSpace is under the threshold
    void testNoForgetAtoms() {
        // Create atoms with high truth values
        Handle atom1 = atomSpace->add_node(CONCEPT_NODE, "Atom1");
        atom1->setTruthValue(SimpleTruthValue::createTV(0.8, 0.9));
        Handle atom2 = atomSpace->add_node(CONCEPT_NODE, "Atom2");
        atom2->setTruthValue(SimpleTruthValue::createTV(0.7, 0.8));

        // No links needed for this test

        // Set ForgettingAgent parameters
        forgettingAgent->maxSize = 5;  // Set max AtomSpace size high enough to prevent removal
        forgettingAgent->forgetThreshold = 0.3;  // Forget threshold

        // Run the ForgettingAgent
        forgettingAgent->run();

        // Ensure the AtomSpace size hasn't changed
        HandleSeq remainingAtoms;
        atomSpace->get_handles_by_type(std::back_inserter(remainingAtoms), ATOM, true);

        // Assert that no atoms are removed
        TS_ASSERT_EQUALS(remainingAtoms.size(), 2);  // Both atoms should remain
    }

    // Test case where an atom marked as non-disposable should not be forgotten
    void testNonDisposableAtom() {
        // Create an atom with a low truth value, below the threshold
        Handle atom = atomSpace->add_node(CONCEPT_NODE, "Atom1");
        atom->setTruthValue(SimpleTruthValue::createTV(0.1, 0.5));

        // Mark the atom as non-disposable
        atom->set_vlti(AttentionValue::NONDISPOSABLE);

        // Set ForgettingAgent parameters
        forgettingAgent->maxSize = 1;  // Restrict to 1 atom in AtomSpace
        forgettingAgent->forgetThreshold = 0.3;  // Forget threshold

        // Run the ForgettingAgent
        forgettingAgent->run();

        // Ensure the non-disposable atom is not removed
        HandleSeq remainingAtoms;
        atomSpace->get_handles_by_type(std::back_inserter(remainingAtoms), ATOM, true);

        // Assert that the atom is still present in the AtomSpace
        TS_ASSERT_EQUALS(remainingAtoms.size(), 1);  // Atom1 should remain, as it is non-disposable
    }
};
