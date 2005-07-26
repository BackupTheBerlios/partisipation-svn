indent example_input.c -o example_std.c
indent -kr example_input.c -o example_kr.c
indent -orig example_input.c -o example_orig.c
indent -l80 -nut -i4 \
  -bad -bap \
  -fca -fc1 -c33 -cd33 -cp33 -lc80 \
  -bbb -nsob -d0 -cdb -nsc \
  -br -ce -cdw -cli4 -cbi0 \
  -nss -npcs -cs -nbs \
  -saf -sai -saw -nprs \
  -bc -nbfda -nbfde -psl -brf \
  -lp -ip4 -ppi4 -nlps -bbo \
  example_input.c -o example_sipoks.c
