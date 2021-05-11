DROP TABLE IF EXISTS test_table;

CREATE TABLE test_table(text_field  text);

INSERT INTO test_table (text_field) VALUES ('This is text data');

INSERT INTO test_table (text_field) VALUES ('It''s escaped text data');
