Those are "exports". Scripts here must NOT use any globals, except 
unique named functions. Because those will be automatically preloaded 
into ANY engine created (order of load is undefined).

Actions called from "script" will be persistent until bound item in the 
tree exists. I.e. engine is not restarted or recreated.

So those can be used to add additional functions per item in tree.
