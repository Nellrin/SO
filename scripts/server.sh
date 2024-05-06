
server_input="$@"

clear
echo "+────────────+"
echo "|Orchestrator|"
echo "+────────────+"
echo ""
valgrind ./bin/orchestrator $server_input