-- QUERY4

-- Laura de Paz and Paula Samper

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
         -- This subquery counts the number of times a language is related to a film
         -- (the language in which the film has been filmed),and chooses the most popular one
WHERE  top_language.language_id = language.language_id;
-- We relate the most popular language with the language.id
