
Tiny RADIUS Client Library
Copyright (C) 2021 David M. Syzdek <david@syzdek.net>.
All rights reserved.

RADIUS Protocol Support
=======================

RADIUS Packet Types
-------------------

   * [ ]   1      Access-Request               [RFC2865]
   * [ ]   2      Access-Accept                [RFC2865]
   * [ ]   3      Access-Reject                [RFC2865]
   * [ ]   4      Accounting-Request           [RFC2865]
   * [ ]   5      Accounting-Response          [RFC2865]
   * [ ]   6      Accounting-Status            [RFC2882]
   * [ ]   7      Password-Request             [RFC2882]
   * [ ]   8      Password-Ack                 [RFC2882]
   * [ ]   9      Password-Reject              [RFC2882]
   * [ ]  10      Accounting-Message           [RFC2882]
   * [ ]  11      Access-Challenge             [RFC2865]
   * [ ]  12      Status-Server (experimental) [RFC2865]
   * [ ]  13      Status-Client (experimental) [RFC2865]
   * [ ]  21      Resource-Free-Request        [RFC2882]
   * [ ]  22      Resource-Free-Response       [RFC2882]
   * [ ]  23      Resource-Query-Request       [RFC2882]
   * [ ]  24      Resource-Query-Response      [RFC2882]
   * [ ]  25      Alternate-Resource-
   * [ ]  26      NAS-Reboot-Request           [RFC2882]
   * [ ]  27      NAS-Reboot-Response          [RFC2882]
   * [ ]  28      Reserved
   * [ ]  29      Next-Passcode                [RFC2882]
   * [ ]  30      New-Pin                      [RFC2882]
   * [ ]  31      Terminate-Session            [RFC2882]
   * [ ]  32      Password-Expired             [RFC2882]
   * [ ]  33      Event-Request                [RFC2882]
   * [ ]  34      Event-Response               [RFC2882]
   * [ ]  40      Disconnect-Request           [DynAuth]
   * [ ]  41      Disconnect-ACK               [DynAuth]
   * [ ]  42      Disconnect-NAK               [DynAuth]
   * [ ]  43      CoA-Request                  [DynAuth]
   * [ ]  44      CoA-ACK                      [DynAuth]
   * [ ]  45      CoA-NAK                      [DynAuth]
   * [ ]  50      IP-Address-Allocate          [RFC2882]
   * [ ]  51      IP-Address-Release           [RFC2882]
   *     250-253  Experimental Use
   * [ ] 254      Reserved
   * [ ] 255      Reserved                     [RFC2865]

RADIUS Attributes
-----------------

   * [x] RFC 2865
   * [x] RFC 2866
   * [ ] RFC 2867
   * [ ] RFC 2868
   * [ ] RFC 2869

RADIUS Attribute Data Types
---------------------------

   * [ ] text/string             (RFC 8044/FreeRADIUS)
   * [ ] string/octets           (RFC 8044/FreeRADIUS)
   * [ ] address/ipaddr          (RFC 2865/FreeRADIUS)
   * [ ] time/date               (RFC 8044/FreeRADIUS)
   * [ ] integer/integer         (RFC 8044/FreeRADIUS)
   * [ ] ipv6addr/ipv6addr       (RFC 8044/FreeRADIUS)
   * [ ] ipv6prefix/ipv6prefix   (RFC 8044/FreeRADIUS)
   * [ ] ifid/ifid               (RFC 8044/FreeRADIUS)
   * [ ] integer64integer64      (RFC 6929/FreeRADIUS)
   * [ ] ----/ether              (no RFC/FreeRADIUS)
   * [ ] ----/abinary            (no RFC/FreeRADIUS)
   * [ ] ----/byte               (no RFC/FreeRADIUS)
   * [ ] ----/short              (no RFC/FreeRADIUS)
   * [ ] ----/signed             (no RFC/FreeRADIUS)
   * [ ] tlv/tlv                 (RFC 6929/FreeRADIUS)
   * [ ] ipv4prefix/ipv4prefix   (RFC 8044/FreeRADIUS)
   * [ ] evs/----                (RFC 6929/No Support FreeRADIUS 3.0.25)
