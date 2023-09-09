#!/system/bin/sh
/system/bin/iptables -w -A oem_out -p tcp --tcp-flags SYN SYN -j TCPMSS --clamp-mss-to-pmtu
/system/bin/ip6tables -w -A oem_out -p tcp --tcp-flags SYN SYN -j TCPMSS --clamp-mss-to-pmtu
