-- QUERY4

-- Language in which most of the films have been filmed. The query must return the language.name attribute.
-- If there is a tie between several languages, all languages in which the greatest number of films have
-- been filmed should appear in the answer.

SELECT language.name
FROM   language,
       (SELECT language_id,
               Count(*) AS num
        FROM   film
        GROUP  BY language_id
        ORDER  BY num DESC
        LIMIT  1) AS top_language
WHERE  top_language.language_id = language.language_id; 
