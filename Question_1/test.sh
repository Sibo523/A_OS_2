
# user input error
echo -n "" |./ttt "1234567891"
echo -n "" |./ttt "012345678"

# #user win
echo -n "9876\n" |./ttt "123456789" 
echo -n "99876\n" |./ttt "123456789"
echo -n "9187\n" |./ttt "123456789"
#computer win
echo -n "986\n" |./ttt "123456789" 
echo -n "9148\n" |./ttt "123456789"
#draw
echo -n "24679\n" |./ttt "123456789"

gcov ttt.c
cat ttt.c.gcov