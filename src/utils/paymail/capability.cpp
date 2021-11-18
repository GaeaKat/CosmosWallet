// Copyright (c) 2021 Katrina Knight
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#include <utils/paymail/capability.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace Gigamonkey::Bitcoin::Paymail {

    std::string getCapabilityId(Capability cap) {
        {
            switch (cap) {
                case basicAddressResolution:
                    return "759684b1a19a";
                case p2pPaymentDestination:
                    return "2a40af698840";
                case p2pTransactions:
                    return "5f1323cddf31";
                case paymentDestination:
                    return "paymentDestination";
                case payToProtocolPrefix:
                    return "7bd25e5a1fc6";
                case pki:
                    return "pki";
                case pkiAlternate:
                    return "0c4339ef99c2";
                case publicProfile:
                    return "f12f968c92d6";
                case receiverApprovals:
                    return "3d7c2ca83a46";
                case senderValidation:
                    return "6745385c3fc0";
                case verifyPublicKeyOwner:
                    return "a9f510c16bde";
                default:
                    return "";
                case p2pPaymentDestinationWithToken:
                    return "f792b6eff07a";
                case sfpAssetInformation:
                    return "1300361cb2d4";
                case sfpAuthorizeAction:
                    return "95dddb461bff";
                case sfpBuildAction:
                    return "189e32d93d28";
            }
        }
    }

    Capability getCapabilityFromId(std::string id) {
        if(boost::iequals(id,"759684b1a19a"))
            return basicAddressResolution;
        else if(boost::iequals(id,"2a40af698840"))
            return p2pPaymentDestination;
        else if(boost::iequals(id,"5f1323cddf31"))
            return p2pTransactions;
        else if(boost::iequals(id,"paymentDestination"))
            return paymentDestination;
        else if(boost::iequals(id,"7bd25e5a1fc6"))
            return payToProtocolPrefix;
        else if(boost::iequals(id,"pki"))
            return pki;
        else if(boost::iequals(id,"0c4339ef99c2"))
            return pkiAlternate;
        else if(boost::iequals(id,"f12f968c92d6"))
            return publicProfile;
        else if(boost::iequals(id,"3d7c2ca83a46"))
            return receiverApprovals;
        else if(boost::iequals(id,"6745385c3fc0"))
            return senderValidation;
        else if(boost::iequals(id,"a9f510c16bde"))
            return verifyPublicKeyOwner;
        else if(boost::iequals(id,"f792b6eff07a"))
            return p2pPaymentDestinationWithToken;
        else if(boost::iequals(id,"1300361cb2d4"))
            return sfpAssetInformation;
        else if(boost::iequals(id,"95dddb461bff"))
            return sfpAuthorizeAction;
        else if(boost::iequals(id,"189e32d93d28"))
            return sfpBuildAction;
    }
}
