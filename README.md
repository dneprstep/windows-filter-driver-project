Документация

Для работы драйвера нужна поддержка работы фильтр-менеджера. Поддерживаемые операционные системы: Windows 2000 SP4 with security rollup, Windows XP SP2, Windows Server 2003 SP1 и более поздние версии.

Работа драйвера осуществляется с помощью перехвата IRP запросов из обращения к директории и анализа этого запроса. Схематически это выглядит так.
 
Драйвер подключается к стеку драйверов Filter manager и обрабатывает поступающие к нему FLT_POSTOP_CALLBACK запросы IRP_MJ_DIRECTORY_CONTROL.
Для запуска фильтра необходимо установить драйвер с помощью inf файла, а затем выполнить его запуск через фильтр-менеджер(fltmc) 
командой    fltmc load Filter.

Список используемых фильтров находится в файле по адресу: C:\hideMaskFile.mask

Разделителем в списке фильтров служит знак «;». Символы «маски» должны быть заглавные.

Сообщения о работе драйвера выводятся через DbgPrint.

При успешном запуске драйвера выводится информация о статусе открытия файла (успешно или нет), перечень добавленных масок и о статусе запуска драйвера.

При выгрузке – выводит список очищенных масок файлов.



Известные проблемы:

В ОС Windows XP, если файл, который необходимо скрыть находится в корне диска(например С:\) и он считывается первым, то скрывается все содержимое диска.
Перед запуском драйвера конфигурационный файл необходимо закрыть.

