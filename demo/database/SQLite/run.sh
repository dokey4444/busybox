#!/bin/sh

./debug info.db 'create table info ( timestamp int, cpu real, mem real, in_net real, out_net real, in_disk real, out_disk real, progress real);'

./debug info.db 'insert into info values (0, 0, 0, 0, 0, 0, 0, 0);'
./debug info.db 'insert into info values (1, 1, 1, 1, 1, 1, 1, 1);'
./debug info.db 'insert into info values (2, 2, 2, 2, 2, 2, 2, 2);'
./debug info.db 'insert into info values (3, 3, 3, 3, 3, 3, 3, 3);'
./debug info.db 'insert into info values (4, 4, 4, 4, 4, 4, 4, 4);'
./debug info.db 'insert into info values (5, 5, 5, 5, 5, 5, 5, 5);'
./debug info.db 'insert into info values (6, 6, 6, 6, 6, 6, 6, 6);'
./debug info.db 'insert into info values (7, 7, 7, 7, 7, 7, 7, 7);'
./debug info.db 'insert into info values (8, 8, 8, 8, 8, 8, 8, 8);'
./debug info.db 'insert into info values (9, 9, 9, 9, 9, 9, 9, 9);'

./debug info.db 'select * from info;'

rm -f info.db

