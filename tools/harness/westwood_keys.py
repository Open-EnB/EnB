"""Westwood's RSA key for E&B's TCP handshake (the server decrypts the client's RC4 session key with it).

Published long ago with the Net-7 server emulator: trunk/Net7/WestwoodRSA.h of
https://github.com/therealKyp/Earth-and-Beyond-server (WWRSA_N, WWRSA_d, WWRSA_e). The modulus is
also in client.exe. udpreplay.py's handshake check decrypts the EA captures' session keys with it.
"""
# modulus, the server's private exponent, the public exponent
N = int(
        '1038557801480495022106519019573649119384754147938972842042651408377132694563972973669579122557389379'
        '3119489336012297845146104637691941242485732839277543427')
D = int(
        '1008884721438195164332047047585830573116618315140716475127147082423500331862125230796975208608807649'
        '9395823874814123350292603347408732347765156628342107995')
E = 35
