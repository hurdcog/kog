/**
 * @file gnucash_atomspace_adapter.h
 * @brief Header for Gnucash AtomSpace integration
 */

#ifndef OPENCOG_GNUCASH_ATOMSPACE_ADAPTER_H
#define OPENCOG_GNUCASH_ATOMSPACE_ADAPTER_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Handle.h>
#include <string>
#include <vector>

namespace opencog {
namespace gnucash {

/**
 * Data structure for Gnucash account information
 */
struct AccountData {
    std::string name;
    std::string type;
    double balance;
    std::string currency;
};

/**
 * Data structure for Gnucash transaction information  
 */
struct TransactionData {
    std::string from_account;
    std::string to_account;
    double amount;
    std::string date;
    std::string description;
};

/**
 * Adapter class for converting Gnucash data to AtomSpace representations
 */
class GnucashAtomSpaceAdapter {
private:
    AtomSpace* _atomspace;
    
public:
    /**
     * Constructor
     * @param atomspace Pointer to the AtomSpace instance
     */
    explicit GnucashAtomSpaceAdapter(AtomSpace* atomspace);
    
    /**
     * Convert Gnucash account to AtomSpace representation
     * @param account Account data to convert
     * @return Handle to the created account concept node
     */
    Handle convertAccount(const AccountData& account);
    
    /**
     * Convert financial transaction to AtomSpace representation
     * @param transaction Transaction data to convert
     * @return Handle to the created transaction evaluation link
     */
    Handle convertTransaction(const TransactionData& transaction);
    
    /**
     * Apply cognitive attention to account based on significance
     * @param account_handle Handle to the account atom
     * @param threshold Significance threshold for attention
     */
    void applyCognitiveAttention(Handle account_handle, double threshold);
    
    /**
     * Query AtomSpace for financial patterns and insights
     * @return Vector of handles representing discovered patterns
     */
    std::vector<Handle> getFinancialPatterns();
};

} // namespace gnucash
} // namespace opencog

#endif // OPENCOG_GNUCASH_ATOMSPACE_ADAPTER_H