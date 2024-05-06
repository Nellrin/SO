
make clean
make r
make

server_input="$@"

chmod +x scripts/server.sh
chmod +x scripts/client.sh

gnome-terminal --title "Client" -- bash -c "./scripts/client.sh; exec bash"

./scripts/server.sh $server_input


make clean
#make r

#   chmod +x scripts/client.sh 
#   chmod +x scripts/server.sh 
#   ./scripts/server.sh output_folder 8 FCFS
#   ./scripts/set_up.sh output_folder 2 FCFS