rem This tests the example encryption in the c12.22 standard of 03-02-2006
c1222testcbc 3aa9800403300000 8f39aeb8fa9041af 3986f51c3986f51c e
rem encrypted data (edata) should be 40c536708cf600eb
c1222testcbc 40c536708cf600eb 8f39aeb8fa9041af 3986f51c3986f51c d
rem decrypted data (data) should be 3aa9800403300000
c1222testcbc 32e4800404300000 E59B4741A7D4067EAB84BA62C49153A1AA29A0E1E6DE3653 3986F69E3986F69E e
rem encrypted data should be E102CD4D8129D643 
c1222testcbc E102CD4D8129D643 E59B4741A7D4067EAB84BA62C49153A1AA29A0E1E6DE3653 3986F69E3986F69E d
rem decrypted data should be 32e4800404300000 
