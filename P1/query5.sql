--QUERY5

-- Language (of the films) in which a greater number of rentals has been done.
-- The query must return the language.name attribute. If there is a tie between several languages,
-- all languages in which a greater number of rentals has been done must appear in the answer.

SELECT language.name
FROM   language,
       (SELECT film.language_id,
               Count(*)
        FROM   rental,
               inventory,
               film
        WHERE  rental.inventory_id = inventory.inventory_id
               AND inventory.film_id = film.film_id
        GROUP  BY film.language_id
        ORDER  BY film.language_id DESC
        LIMIT  1) AS top_lang
WHERE  top_lang.language_id = language.language_id; 
