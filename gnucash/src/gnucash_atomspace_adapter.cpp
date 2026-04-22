/**
 * @file gnucash_atomspace_adapter.cpp
 * @brief OpenCog AtomSpace adapter for Gnucash financial data
 * 
 * This adapter converts Gnucash financial transactions and accounts
 * into AtomSpace representations for cognitive reasoning.
 */

#include <opencog/gnucash/gnucash_atomspace_adapter.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>

namespace opencog {
namespace gnucash {

GnucashAtomSpaceAdapter::GnucashAtomSpaceAdapter(AtomSpace* atomspace)
    : _atomspace(atomspace)
{
    // Initialize adapter with AtomSpace reference
}

Handle GnucashAtomSpaceAdapter::convertAccount(const AccountData& account)
{
    // Create concept node for account
    std::string account_name = "Account:" + account.name;
    Handle account_node = _atomspace->add_node(CONCEPT_NODE, account_name);
    
    // Add account balance as number node
    Handle balance_node = _atomspace->add_node(NUMBER_NODE, 
                                              std::to_string(account.balance));
    
    // Create evaluation link for balance relationship
    Handle balance_predicate = _atomspace->add_node(PREDICATE_NODE, "has_balance");
    Handle balance_link = _atomspace->add_link(EVALUATION_LINK, 
                                             balance_predicate,
                                             _atomspace->add_link(LIST_LINK,
                                                                account_node,
                                                                balance_node));
    
    return account_node;
}

Handle GnucashAtomSpaceAdapter::convertTransaction(const TransactionData& transaction)
{
    // Create nodes for source and destination accounts
    Handle from_account = _atomspace->add_node(CONCEPT_NODE, 
                                             "Account:" + transaction.from_account);
    Handle to_account = _atomspace->add_node(CONCEPT_NODE,
                                           "Account:" + transaction.to_account);
    Handle amount_node = _atomspace->add_node(NUMBER_NODE,
                                            std::to_string(transaction.amount));
    
    // Create transaction evaluation link
    Handle transaction_predicate = _atomspace->add_node(PREDICATE_NODE, 
                                                       "financial_transaction");
    Handle transaction_link = _atomspace->add_link(EVALUATION_LINK,
                                                 transaction_predicate,
                                                 _atomspace->add_link(LIST_LINK,
                                                                    from_account,
                                                                    to_account,
                                                                    amount_node));
    
    return transaction_link;
}

void GnucashAtomSpaceAdapter::applyCognitiveAttention(Handle account_handle, 
                                                     double threshold)
{
    // Apply high attention to accounts with significant balance changes
    TruthValuePtr high_attention = SimpleTruthValue::createTV(0.9, 0.8);
    account_handle->setTruthValue(high_attention);
    
    // TODO: Integrate with ECAN (Economic Attention Networks)
    // This would involve setting attention values and importance measures
}

std::vector<Handle> GnucashAtomSpaceAdapter::getFinancialPatterns()
{
    // Query AtomSpace for financial patterns and relationships
    std::vector<Handle> patterns;
    
    // Example: Find all accounts with high balances
    // This would use pattern matching to discover financial insights
    
    return patterns;
}

} // namespace gnucash
} // namespace opencog