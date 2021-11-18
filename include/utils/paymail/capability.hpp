// Copyright (c) 2021 Katrina Knight
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_CAPABILITY_H
#define GIGAMONKEY_CAPABILITY_H

#include <string>

namespace Gigamonkey::Bitcoin::Paymail {
    /**
     * Capabilities in general for paymail.
     * Not everyone here is supported.
     */
    enum Capability {
        basicAddressResolution,             // http://bsvalias.org/04-01-basic-address-resolution.html
        p2pPaymentDestination,              // https://docs.moneybutton.com/docs/paymail-07-p2p-payment-destination.html
        p2pPaymentDestinationWithToken,     // https://docs.moneybutton.com/docs/paymail/paymail-11-p2p-payment-destination-tokens.html
        p2pTransactions,                    // https://docs.moneybutton.com/docs/paymail-06-p2p-transactions.html
        paymentDestination,                 // http://bsvalias.org/04-01-basic-address-resolution.html
        payToProtocolPrefix,                // http://bsvalias.org/04-04-payto-protocol-prefix.html
        pki,                                // http://bsvalias.org/03-public-key-infrastructure.html
        pkiAlternate,                       // http://bsvalias.org/03-public-key-infrastructure.html
        publicProfile,                      // https://github.com/bitcoin-sv-specs/brfc-paymail/pull/7/files
        receiverApprovals,                  // http://bsvalias.org/04-03-receiver-approvals.html
        senderValidation,                   // http://bsvalias.org/04-02-sender-validation.html
        sfpAssetInformation,                // https://docs.moneybutton.com/docs/paymail/paymail-08-asset-information.html
        sfpAuthorizeAction,                 // https://docs.moneybutton.com/docs/sfp/paymail-10-sfp-authorise.html
        sfpBuildAction,                     // https://docs.moneybutton.com/docs/sfp/paymail-09-sfp-build.html
        verifyPublicKeyOwner,               // http://bsvalias.org/05-verify-public-key-owner.html
    };


    /**
     * Gets the ID for a capabilitiy
     * @param cap Capability to check id for
     * @return Capability id if valid, or empty string otherwise
     */
    std::string getCapabilityId(Capability cap);

    Capability getCapabilityFromId(std::string id);


}
#endif //GIGAMONKEY_CAPABILITY_H
