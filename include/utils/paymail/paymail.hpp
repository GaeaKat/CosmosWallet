// Copyright (c) 2021 Katrina Knight
// Distributed under the Open BSV software license, see the accompanying file LICENSE.


#ifndef GIGAMONKEY_PAYMAIL_H
#define GIGAMONKEY_PAYMAIL_H

#include <getdns/getdns.h>
#include <boost/function.hpp>

#include <data/cross.hpp>
#include <gigamonkey/types.hpp>
#include "capability.hpp"

namespace Gigamonkey::Bitcoin::Paymail {

    /**
     * Class handling Paymail operations
     */
    class Paymail {
    public:
        Paymail();

    protected:

        getdns_context *ctxt = NULL;

        getdns_transaction_t transaction_id;

        /**
         * Splits a paymail address into user and domain.tld
         * @param address Address to split
         * @return a Pair containing first the user, and second the domain and tld
         */
        std::pair<std::string,std::string> splitAddress(std::string address);

        /**
         * gets the capabilities, and associated urls for a domain
         * @param domain
         * @param ignoreCache
         * @return
         */
        std::map<Capability,std::string> getCapabilities(std::string domain,bool ignoreCache = false);

        /**
         * Gets the  server name and port for the capabilities server
         * @param domain domain to get server for
         * @returns string pointing to the server in the form server:port
         */
        std::string getCapabilityServer(std::string domain);

        std::string fixURL(std::string url,std::string alias,std::string domain);
    public:
        /**
         * Checks if a paymail is valid
         * @note This only checks if it matches. It does not use any validation services currently.
         * @param address Address to check
         * @return true if valid, false otherwise
         */
        static bool isValid(std::string address);

        /**
         * Gets a script corresponding to the paymail address
         * @param address
         * @param sender
         * @param amount
         * @param purpose
         * @param senderName
         * @param signature
         * @return
         */
        bytes getScript(std::string address,std::string sender,int amount=0,std::string purpose="",std::string senderName="",std::string signature="");
    };
}
#endif //GIGAMONKEY_PAYMAIL_H
