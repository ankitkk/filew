# filew
Track files changed/added/deleted for a directory. Computes MD5s and stores the cached values on every run. 

The idea is to provide a faster alternative of the unix command line  

```find ~/repo/ -type f | xargs md5sum```  
