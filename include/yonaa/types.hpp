#pragma once
// TODO(Caleb): Switch from #pragma once to proper ifdef guards

#include <netdb.h>

using address_type         = sockaddr;
using address_size_type    = socklen_t;
using address_family_type  = sa_family_t;
using address_storage_type = sockaddr_storage;
using protocol_type        = int;
using socket_type          = int;
using gai_result_type      = addrinfo;
