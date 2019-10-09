-- QUERY1

-- Laura de Paz and Paula Samper

-- Number of movies rented each year. The query must return two attributes
-- containing the year and the number of films rented. Order the answer by the
-- rental year in ascending order. Hint: the year can be extracted from a “date”
-- variable using the function extract.


SELECT Extract(year FROM rental_date) AS year,
       Count(*)
FROM   rental
GROUP  BY year
ORDER  BY year ASC;

-- We extract the year from the rental date and how many movies were rented that year,
-- and we order them in ascending order 
