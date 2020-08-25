### gilgil netfilter-test

```bash
sudo iptables -F
sudo iptables -A OUTPUT -p tcp -j NFQUEUE
sudo iptables -A INPUT -p tcp -j NFQUEUE
```

### use

```bash
$ sudo ./netfilter-test {hostmain}
$ ex) $ sudo ./netfilter-test test.gilgii.net
$ ex) $ sudo ./netfilter-test google.com
```

### result
