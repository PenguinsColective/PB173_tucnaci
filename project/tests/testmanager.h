/* 
 * File:   testmanager.h
 * Author: jarosm
 *
 * Created on Streda, 2013, október 16, 12:09
 */

#ifndef TESTMANAGER_H
#define	TESTMANAGER_H
#include "minunit.h"
#include <list>



namespace tests {

    /**
     * \brief Abstract class for Test representation
     *        All tests of one component should be methods of 
     *        offspring class.
     */
    class TestCase {
    public:

        virtual ~TestCase() {}
        /**
         * \brief Abstract method which runs all tests of component.
         * 
         * @return NULL iff all tests were succesfull else the error message.
         */
        virtual const char * runTests() = 0;
    };

    /**
     * \brief Class represents manager of all test classes.
     * 
     *        
     */
    class TestManager {
    public:

        ~TestManager();

        /**
         * \brief In this method will be intoduced All testCase offsprings
         *        which should be runned.
         *         
         */
        void prepare();
        /**
         * \brief Starts testing over list of Test Cases.
         */
        void startTesting();

    private:


        std::list<TestCase*> tests;

    };
}
#endif	/* TESTMANAGER_H */

