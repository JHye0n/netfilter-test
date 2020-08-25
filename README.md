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

<img width="562" alt="스크린샷 2020-08-25 오전 11 34 29" src="https://user-images.githubusercontent.com/50125695/91116277-f4036e00-e6c6-11ea-879f-0b633094cd80.png">
<img width="573" alt="스크린샷 2020-08-25 오전 11 34 47" src="https://user-images.githubusercontent.com/50125695/91116292-fe256c80-e6c6-11ea-809a-72e2388a4212.png">
<img width="269" alt="스크린샷 2020-08-25 오전 11 35 06" src="https://user-images.githubusercontent.com/50125695/91116307-09789800-e6c7-11ea-89d3-9dcaf8975e28.png">
<img width="776" alt="스크린샷 2020-08-25 오전 11 35 19" src="https://user-images.githubusercontent.com/50125695/91116318-109fa600-e6c7-11ea-896f-37944c881f9e.png">
<img width="805" alt="스크린샷 2020-08-25 오전 11 35 31" src="https://user-images.githubusercontent.com/50125695/91116343-185f4a80-e6c7-11ea-8feb-48196d97ad31.png">

