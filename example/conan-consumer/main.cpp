#include <sqlcppbridge/sqlcppbridge.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cassert>

class Contact
{
    DECLARE_SQL_ACCESS(Contact);
public:
    Contact() = default;
    Contact(long long id, std::string name, std::string email)
        : id_(id), name_(std::move(name)), email_(std::move(email)) {}

    long long   id()    const { return id_; }
    std::string name()  const { return name_; }
    std::string email() const { return email_; }

private:
    long long   id_    = 0;
    std::string name_;
    std::string email_;
};

DEFINE_SQL_TABLE(contacts, Contact)
{
    bind("ID",    &Contact::id_,    e_db_index_type::Unique),
    bind("NAME",  &Contact::name_),
    bind("EMAIL", &Contact::email_),
};

DEFINE_SQL_DATABASE(contacts_db, 1, Contact)::upgrade_structure(size_t from, size_t)
{
    if (from) drop_all();
}

int main()
{
    const std::string db_dir = "./contacts_demo";
    std::filesystem::create_directories(db_dir);

    using storage_t = sql_bridge::local_storage<sql_bridge::sqlite_adapter>;
    storage_t storage(db_dir);
    sql_bridge::context db(storage["contacts_db"]);

    // Write
    std::vector<Contact> contacts = {
        {1, "Alice",   "alice@example.com"},
        {2, "Bob",     "bob@example.com"},
        {3, "Charlie", "charlie@example.com"},
    };
    db.save(contacts);
    std::cout << "Saved " << contacts.size() << " contacts.\n";

    // Read all back
    std::vector<Contact> loaded;
    db.load(loaded);
    assert(loaded.size() == contacts.size());
    std::cout << "Loaded " << loaded.size() << " contacts:\n";
    for (auto const& c : loaded)
        std::cout << "  [" << c.id() << "] " << c.name() << " <" << c.email() << ">\n";

    // Targeted load
    Contact alice;
    db.load(alice, "WHERE ID=1");
    assert(alice.name() == "Alice");
    std::cout << "Targeted load: " << alice.name() << " — ok\n";

    return 0;
}
