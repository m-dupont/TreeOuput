Write a numpy array like a ROOT TTree array :


```

OutputNumpyTreeFile h;
h.open("/tmp/z5.npy");
int p = 0;

h.write_variable("my_p", &p);
h.write_header();

for (i = 0; i < 500; ++i)
{
    p = i;
    h.fill();


}

h.close()


```
