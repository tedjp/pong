Pong
====

Pong allows your computer to play pong with another computer.

It's not really interactive, unless you want to lose.

Each endpoint sends a ping every 1±½ second. If one party doesn't receive
a ping within 2 seconds, it means the opponent dropped the ping and the
opponent loses the point.

Usage
=====

    ./pong <remote-address>
