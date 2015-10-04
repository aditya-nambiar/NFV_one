top -p $(pgrep mme | tr \\n , | sed 's/,//') -bn 3 -d 1 
| grep "root" 
| awk 'BEGIN{count=0;sum=0;}{sum = sum + $9;count++;}END{print sum/count;}'