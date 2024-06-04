#defult just runs the program when the some user enters but doesn't give him perms
# I\O from and to the server
#enter 1 3 4 6 0 9
./mync -e "ttt 123456789"  TCPS4050
./mync -e "ttt 123456789"  TCPC4050
./mync -e "ttt 123456789"  UDPS4050
./mync -e "ttt 123456789"  UDPC4050
#####################################################
#error cause there's not ttt
./mync -e "tt 123456789" -b TCPS4050
./mync -e "tt 123456789" -b TCPC4050

#game with both TCP
./mync -e "ttt 123456789" -b TCPS4050
./mync -e "ttt 123456789" -b TCPC4050

#chat with both 
./mync "ttt 123456789" -b TCPS4050
./mync "ttt 123456789" -b TCPC4050

#game with input output
./mync -e "ttt 123456789" -i TCPS4050
./mync -e "ttt 123456789" -o TCPC4050

#game with input output
./mync -e "ttt 123456789" -o TCPS4050
./mync -e "ttt 123456789" -b TCPC4050
####################################################

#chat with both  UDP 
./mync "ttt 123456789" -b UDPS4050
./mync "ttt 123456789" -b UDPC4050
#with timeout
./mync -e "ttt 123456789" -i UDPS4050 -t-10
./mync -e "ttt 123456789" -b UDPS4050 -t10
./mync -e "ttt 123456789" -b UDPC4050 -t10

#game with input output
./mync -e "ttt 123456789" -i UDPS4050
./mync -e "ttt 123456789" -o UDPC4050
####################################################

#game UDSSDshalom I/O
./mync -e "ttt 123456789" -i UDSSDshalom
./mync -e "ttt 123456789" -o UDSCDshalom

#game UDSSDshalom bb #not working
./mync -e "ttt 123456789" -b UDSSSshalom
./mync -e "ttt 123456789" -b UDSCSshalom

#chat UDSSDshalom I/O
./mync "ttt 123456789" -b UDSSSshalom
./mync "ttt 123456789" -b UDSCSshalom
####################################################
