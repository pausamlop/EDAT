-- QUERY5

-- Laura de Paz and Paula Samper

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
        ORDER  BY Count(*)  DESC
        LIMIT  1) AS top_lang
        -- In this subquery we relate the rentals with the films, to find out the language in
        -- which the rental has been done, counts them, and chooses the most popular one.
        -- We call it top_lang
WHERE  top_lang.language_id = language.language_id;
-- We relate top_lang with language.id
