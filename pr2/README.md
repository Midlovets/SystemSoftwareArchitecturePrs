# Практична робота №2  
**Тема:** Аналіз ELF-файлів, процесів і пам’яті в Linux

## Мета роботи  
Ознайомитись зі структурою ELF-файлів, навчитися аналізувати пам'ять і процеси за допомогою інструментів командного рядка Linux, а також дослідити способи роботи з пам’яттю під час виконання програм.

---

## Виконання завдань  

### Завдання 1: Аналіз ELF-файлу  
У першому завданні ми досліджували структуру ELF-файлу за допомогою інструментів size та readelf. Було проаналізовано такі сегменти як:

`.text`: вміст цього сегмента містить скомпільований код програми. Його розмір залежить від кількості та складності функцій у програмі.

`.data`: тут зберігаються ініціалізовані глобальні змінні, і, відповідно, зміна цих змінних безпосередньо впливає на розмір цього сегмента.

`.bss`: цей сегмент включає неініціалізовані змінні. Цікаво, що його розмір визначається лише в процесі виконання програми, і в статичному аналізі він може бути порожнім.

Завдяки цьому аналізу можна зрозуміти, як різні параметри компіляції (наприклад, з опціями `debug` чи `optimize`) можуть впливати на розмір цих сегментів.

---

### Завдання 2.2: Аналіз стеку процесу  
У завданні 2.2 ми виконували моніторинг і аналіз стеку процесу через інструменти `gstack` і `gdb`. Це дозволяє отримати інформацію про поточний стан виконання програми на момент її зупинки, а також визначити поточну команду, яку виконує процес.


---

### Завдання 2.3: Дослідження роботи лічильника команд (IP)  
Перевірено можливість обійти виконання програми без IP.  
Висновок:
- IP необхідний для виконання інструкцій.
- Стек зберігає адреси повернення та локальні змінні, але не може керувати потоком виконання самостійно.

---

### Завдання 2.4: Робота з mremap  
Завдання 2.4 було присвячене дослідженню техніки зміни сегмента коду через використання системного виклику `mremap`. Це дозволяє модифікувати ділянки пам'яті, що містять код програми, що є важливим для реалізації певних видів атак, таких як переписування пам'яті під час виконання.

---

### Завдання 2.5: Повторне порівняння роботи IP та стеку  
Ні, у загальному випадку неможливо повністю замінити лічильник команд (IP) лише вершиною стека, тому що:
1. IP вказує на поточну інструкцію, яка виконується. Стек — лише допоміжна структура для збереження адрес повернення при викликах функцій (і ще деяких речей — локальних змінних, регістрів).
2. Процесор читає інструкції із памʼяті послідовно за IP. Без IP він не знає, де шукати наступну інструкцію.
3. Стек не є основним механізмом керування потоком — він лише зберігає адреси повернення. Він не містить всі інструкції, які слід виконати.
---

### Варіантне Завдання 15: Вимірювання часу доступу до сегментів пам’яті  
Було реалізовано програму для вимірювання часу доступу до:
- **Стеку**
- **Купи**
- **Глобальної змінної**
- **Константи**
- **Функції**

Отримані результати дозволяють оцінити різницю у швидкості доступу до різних сегментів пам’яті.

---

## Висновки  
Під час виконання практичної роботи:
- Здобуто практичні навички аналізу структури ELF-файлів.
- Розглянуто сегменти `.text`, `.data`, `.bss` та їхній вміст.
- Досліджено роботу стеку, регістрів і пам’яті під час виконання програм.
- Навченося працювати з `gstack`, `gdb`, `readelf`, `size`.
- Виявлено принципову важливість лічильника команд (IP) для виконання програм.
- Оцінено час доступу до різних сегментів пам’яті.

---
