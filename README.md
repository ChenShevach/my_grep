# my_grep

<h2>
Required operating System: Linux
</h2>




<ul>
    <li>
        This program is designed to perform the same outputs as the original linux grep function would perform for the following supported inputs.<br>
    </li>
    <li>
        The program supports the following flags (and their combinations): A,n,c,i,v,b,x and E.
    </li>   
    <li>
        The program can be called either by regular call or by pipping.
    </li>   
    <li>
        E flag supports the following options: dot,or and range (without nested expressions).
    </li>
    <li>
        E flag requires the use in quotes.
    </li> 

</ul>

<div>
   <h2>
      Examples:
   </h2>

   <ul>
       <li>
           ./my_grep call -A 3 ./input.txt
       </li> 
       <li>
           ./my_grep call -i -v ./input.txt
       </li>
       <li>
           ./my_grep -c call -i -v ./input.txt -x
       </li> 
       <li>
	  cat ./input.txt | ./my_grep call -b
       </li> 
       <li> 
	./my_grep -E "(ca|cb).[a-e]" ./input.txt
       </li>  
   </ul>
</div>
