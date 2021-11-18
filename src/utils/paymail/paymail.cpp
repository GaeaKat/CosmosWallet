// Copyright (c) 2021 Katrina Knight
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#include <utils/paymail/paymail.hpp>
#include <data/networking/http.hpp>
#include <ctre.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <arpa/inet.h>
#include <uriparser/Uri.h>

namespace Gigamonkey::Bitcoin::Paymail {



    bytes Paymail::getScript(std::string address,std::string sender, int amount,std::string purpose,std::string senderName,std::string signature) {
        auto cur=splitAddress(address);
        if(cur.first!="") {
            std::map<Capability, std::string> capabilities = getCapabilities(cur.second);
            if (capabilities.count(Capability::paymentDestination) == 0) {
                return bytes();
            }
            std::string url = capabilities[Capability::paymentDestination];
            url = fixURL(url, cur.first, cur.second);
            json j;
            j["senderHandle"]=sender;
            if(amount!=0)
                j["amount"]=amount;
            if(!purpose.empty())
                j["purpose"]=purpose;
            if(!senderName.empty())
                j["senderName"]=senderName;
            if(!signature.empty())
                j["signature"]=signature;
            time_t now;
            time(&now);
            char buf[sizeof "2021-10-08T07:07:09z"];
            strftime(buf,sizeof buf,"%FT%TZ", gmtime(&now));
            j["dt"]=std::string(buf);
            data::networking::http http;
            UriUriA uri;
            const char **errorPos;
            if(uriParseSingleUriA(&uri,url.c_str(),errorPos)) {
                // todo: should this throw an error?
                return bytes();
            }
            std::map<data::networking::http::header,std::string> headers;
            headers[data::networking::http::header::content_type]="application/json";
            try {
                std::string port=data::networking::fromRange(uri.scheme);

                auto ret = http.POST(data::networking::fromRange(uri.hostText), data::networking::fromList(uri.pathHead, "/"),port, {}, headers,
                                     j.dump(4, ' ', true));
                auto retJson = json::parse(ret);
                std::cout << "Returned " << ret << std::endl;
                if (retJson.contains("output")) {
                    std::string outputString = retJson["output"];
                    data::encoding::hex::string hexString(outputString);
                    return static_cast<bytes>(hexString);
                    uriFreeUriMembersA(&uri);
                } else {
                    uriFreeUriMembersA(&uri);
                    return bytes();
                }
            }
            catch(std::exception e) {
                std::cout << "Exception" << std::endl;
                return bytes();
            }
            // warning: Should never get here, should I throw?
        }
        return bytes();
    }


    bool Paymail::isValid(const std::string address) {
        static constexpr auto pattern = ctll::fixed_string{ "^[a-zA-Z0-9_.+\\-]+@[a-zA-Z0-9\\-]+\\.[a-zA-Z0-9\\-.]+$" };
        return ctre::match<pattern>(address );
    }

    std::pair<std::string, std::string> Paymail::splitAddress(std::string address) {
        if(isValid(address))
        {
            std::vector<string> split;
            boost::split(split, address,boost::is_any_of("@"));
            if(split.size()!=2)
                return {};
            else
                return {split[0],split[1]};
        }
        return {};
    }

    std::map<Capability, std::string> Paymail::getCapabilities(std::string domain, bool ignoreCache) {
        std::string capability_domain = this->getCapabilityServer(domain);
        data::networking::http http;
        std::map<Capability, std::string> results;
        auto start=capability_domain.find(":");
        std::string url = capability_domain.substr(0,start);
        try {
            std::map<boost::beast::http::field,string> header;
            header[boost::beast::http::field::content_type]="application/json";
            auto tmp = http.GET(url, "/.well-known/bsvalias","https",{},header);
            auto json = json::parse(tmp);
            for(auto& [key,value] : json["capabilities"].items()) {
                Capability cap=getCapabilityFromId(key);
                std::string val=to_string(value);
                if(val[0] == '"') {
                    val.erase(0,1);
                }
                if(val[val.size()-1] == '"') {
                    val.erase(val.size()-1,1);
                }
                results[cap]=val;
            }
        }
        catch(const std::exception& exception){
            return std::map<Capability, std::string>();
        }
        return results;
    }

    Paymail::Paymail(){
        getdns_return_t r;
        if((r = getdns_context_create(&ctxt,1))) {
            fprintf( stderr, "Could not create context: %u\n"
                    , r);
        } else if ((r = getdns_context_set_resolution_type(
                ctxt, GETDNS_RESOLUTION_STUB)))
            fprintf( stderr, "Could not set stub resolution modus: %u\n"
                    , r);
    }

    std::string Paymail::getCapabilityServer(std::string domain) {
        std::string full_domain="_bsvalias._tcp.";
        full_domain.append(domain);
        full_domain.append(".");
        std::string output="";
        bool valid_srv=false;
        getdns_return_t r;
        getdns_dict* exts=getdns_dict_create();
        getdns_dict* results = nullptr;
//        if ((r = getdns_dict_set_int(exts, "dnssec_return_status"
//                , GETDNS_EXTENSION_TRUE))
//            ||  (r = getdns_dict_set_int(exts, "dnssec_roadblock_avoidance"
//                , GETDNS_EXTENSION_TRUE)))
//            fprintf( stderr, "Could not populate extensions dict: %u\n"
//                    , r);
        output=domain.append(":443");
        if((r = getdns_service_sync(this->ctxt,full_domain.c_str(),exts,&results))) {
            valid_srv = false;
        }
        else {
            valid_srv=true;
        }
        if(valid_srv) {
            getdns_bindata *domain_target;
            if ((r = getdns_dict_get_bindata(results, "/srv_addresses/0/domain_name", &domain_target))) {
                valid_srv = false;
            } else {

                char *name = nullptr;
                getdns_convert_dns_name_to_fqdn(domain_target, &name);

                std::string domain(name);
                uint32_t domain_port;
                if ((getdns_dict_get_int(results, "/srv_addresses/0/port", &domain_port))) {
                    domain_port = 443;
                    valid_srv = false;
                }
                domain.pop_back();
                domain.append(":");
                domain.append(std::to_string(domain_port));
                if(valid_srv) {
                    output=domain.append("");
                }
            }


//            // todo: Check DNSSEC
//            uint32_t dnssec;
//            if ((r = getdns_dict_get_int(results, "/replies_tree/0/dnssec_status", &dnssec))) {
//                // todo: check domain is the same as domain or www.domain
//                std::cerr << "I got error: " << r;
//            } else {
//                if (dnssec != GETDNS_DNSSEC_SECURE) {
//
//                }
//            }

        }
        getdns_dict_destroy(results);
        getdns_dict_destroy(exts);
        return output;
    }

    std::string Paymail::fixURL(std::string url,std::string alias,std::string domain) {
        boost::replace_all(url,"{alias}",alias);
        boost::replace_all(url,"{domain.tld}",domain);
        return url;
    }




}