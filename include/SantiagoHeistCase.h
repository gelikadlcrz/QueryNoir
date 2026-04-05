#pragma once
#include "ICase.h"


class SantiagoHeistCase : public ICase {
public:

    // ─────────────────────────────────────────────────────────────────────────
    // 1. INITIALIZATION (Setup the story, tables, and clues)
    // ─────────────────────────────────────────────────────────────────────────
    void init(GameState& state) override {
        // 1. Set the Case Narrative Data
        Case c;
        c.id = "heist";
        c.title = "THE SANTIAGO HEIST";
        c.help_objective = "2.4 Billion credits were drained from the Central Reserve. Identify the employee who provided the digital vault override and their offshore handler. Collect 8 clues, then click ACCUSE.";
        c.accusation_prompt = "The vault was breached from within. Type the full name of the employee who authorized the 'OVERRIDE' or their external accomplice.";
        c.resolved_title = "THE SANTIAGO HEIST -- RESOLVED";
        c.charged_name   = "Arthur Pendelton (E-44) -- IT Lead";
        c.accessory_name = "Julian Vane (EXT-9) -- Syndicate Handler";
        c.resolution_text = 
            "The Santiago Central Reserve was bled dry not by explosives, but by a single line of code. "
            "Arthur Pendelton, the bank's own IT Lead, utilized his 'MASTER' clearance to trigger a "
            "remote vault override at 02:14 AM.\n\n"
            "To mask the physical entry, Pendelton used his terminal (IP 192.168.1.44) to disable "
            "Camera 04 in the Server Room exactly sixty seconds prior to the breach. He then "
            "routed the stolen 2.4B credits through a series of rapid-fire transfers to 'Ouroboros Holdings'.\n\n"
            "Julian Vane, an external broker for the Vane Syndicate, coordinated the offshore "
            "laundering, taking a 15% fee before splitting the remainder into Pendelton's private "
            "crypto-wallet. The credits are gone, but the digital fingerprints remained.";

        state.set_current_case(c);

        // 2. Set up the Tables
        state.tables() = {
            {"employees",       "STAFF ROSTER",       true,  "", {}, "👤"},
            {"vault_logs",      "VAULT ACCESS",       true,  "", {}, "🔓"},
            {"terminal_traffic","NETWORK TRAFFIC",    true,  "", {}, "📡"},
            {"camera_grid",     "SECURITY CAMERAS",   false, "Find the 'OVERRIDE' in vault_logs", {}, "👁️"},
            {"access_badges",   "BADGE REGISTRY",     false, "Find which camera was 'DISABLED'", {}, "💳"},
            {"wire_transfers",  "FINANCIALS",         false, "Identify the IP that disabled the camera", {}, "$"},
            {"offshore_accounts","OFFSHORE DATA",      false, "Find the 2.4B transfer in wire_transfers", {}, "🏝️"},
            {"decrypted_comm",  "DECRYPTED MESSAGES", false, "Trace the Ouroboros Holdings destination", {}, "⚿"},
        };

        // 3. Set up the Clues
        state.clues().clear();

        Clue c1; c1.id = "c1"; c1.title = "The Midnight Override";
        c1.hint = "Check vault_logs for any unauthorized access status.";
        c1.description = "A manual 'OVERRIDE' was triggered at 02:14:00. This bypassed the dual-key requirement.";
        c1.condition.sql_must_contain = {"VAULT_LOGS"};
        c1.condition.result_must_contain_cell = "OVERRIDE";
        state.clues().push_back(c1);

        Clue c2; c2.id = "c2"; c2.title = "Blind Spot";
        c2.hint = "Query camera_grid. Was any hardware offline during the breach?";
        c2.description = "CAM-04 (Server Room) was remotely 'DISABLED' at 02:13:00, exactly one minute before the vault opened.";
        c2.condition.sql_must_contain = {"CAMERA_GRID", "DISABLED"};
        c2.condition.result_must_contain_cell = "CAM-04";
        state.clues().push_back(c2);

        Clue c3; c3.id = "c3"; c3.title = "The Rogue Terminal";
        c3.hint = "Check terminal_traffic for the IP that sent the disable command to CAM-04.";
        c3.description = "The command to disable the security grid originated from IP 192.168.1.44.";
        c3.condition.sql_must_contain = {"TERMINAL_TRAFFIC", "CAM-04"};
        c3.condition.result_must_contain_cell = "192.168.1.44";
        state.clues().push_back(c3);

        Clue c4; c4.id = "c4"; c4.title = "IT Lead Clearance";
        c4.hint = "Match the rogue IP 192.168.1.44 to an employee in the roster.";
        c4.description = "IP 192.168.1.44 is assigned to Arthur Pendelton (E-44), the IT Lead.";
        c4.condition.sql_must_contain = {"EMPLOYEES", "192.168.1.44"};
        c4.condition.result_must_contain_cell = "Arthur Pendelton";
        state.clues().push_back(c4);

        Clue c5; c5.id = "c5"; c5.title = "The 2.4 Billion Credit Drain";
        c5.hint = "Search wire_transfers for the stolen amount.";
        c5.description = "A massive transfer of 2,400,000,000 credits was sent to 'Ouroboros Holdings'.";
        c5.condition.sql_must_contain = {"WIRE_TRANSFERS"};
        c5.condition.result_must_contain_cell = "2400000000";
        state.clues().push_back(c5);

        Clue c6; c6.id = "c6"; c6.title = "Offshore Shell";
        c6.hint = "Investigate offshore_accounts for Ouroboros Holdings.";
        c6.description = "Ouroboros Holdings is a shell company managed by Julian Vane (EXT-9).";
        c6.condition.sql_must_contain = {"OFFSHORE_ACCOUNTS", "OUROBOROS"};
        c6.condition.result_must_contain_cell = "Julian Vane";
        state.clues().push_back(c6);

        Clue c7; c7.id = "c7"; c7.title = "The Master Key";
        c7.hint = "Check access_badges for who modified the MASTER clearance.";
        c7.description = "Arthur Pendelton (E-44) granted himself 'MASTER' vault clearance two hours before the heist.";
        c7.condition.sql_must_contain = {"ACCESS_BADGES", "MASTER"};
        c7.condition.result_must_contain_cell = "E-44";
        state.clues().push_back(c7);

        Clue c8; c8.id = "c8"; c8.title = "The Handshake";
        c8.hint = "Read the decrypted_comm. Look for the 'Santiago' keyword.";
        c8.description = "Pendelton to Vane: 'The Santiago vault is open. Initiate the Ouroboros routing now.'";
        c8.condition.sql_must_contain = {"DECRYPTED_COMM"};
        c8.condition.result_must_contain_cell = "Ouroboros routing";
        state.clues().push_back(c8);

        // 4. Set App Status & Typewriter Narratives
        state.app().status = GameStatus::ACTIVE;
        state.push_narrative(NarrativeType::SYSTEM, "FORENSICS TERMINAL v4.7 — CASE: THE SANTIAGO HEIST — INITIALISED");
        state.push_narrative(NarrativeType::MONOLOGUE, "The Central Reserve was drained in under four minutes. No alarms, no broken glass.");
        state.push_narrative(NarrativeType::MONOLOGUE, "The vault thinks the entry was authorized. I need to prove it wasn't.");
        state.push_narrative(NarrativeType::HINT, "Start with vault_logs. Find the exact timestamp of the digital breach.");
    }

    std::string get_sql_script() override {
        return R"SQL(
        CREATE TABLE IF NOT EXISTS employees (
            emp_id      TEXT PRIMARY KEY,
            name        TEXT,
            role        TEXT,
            ip_addr     TEXT,
            status      TEXT
        );
        CREATE TABLE IF NOT EXISTS vault_logs (
            log_id      INTEGER PRIMARY KEY,
            ts          TEXT,
            action      TEXT,
            status      TEXT,
            auth_id     TEXT
        );
        CREATE TABLE IF NOT EXISTS terminal_traffic (
            id          INTEGER PRIMARY KEY,
            ts          TEXT,
            src_ip      TEXT,
            dest_obj    TEXT,
            command     TEXT
        );
        CREATE TABLE IF NOT EXISTS camera_grid (
            cam_id      TEXT PRIMARY KEY,
            location    TEXT,
            status      TEXT,
            last_ping   TEXT
        );
        CREATE TABLE IF NOT EXISTS access_badges (
            badge_id    TEXT PRIMARY KEY,
            holder_id   TEXT,
            clearance   TEXT,
            last_mod    TEXT
        );
        CREATE TABLE IF NOT EXISTS wire_transfers (
            txn_id      TEXT PRIMARY KEY,
            ts          TEXT,
            amount      REAL,
            destination TEXT,
            flag        TEXT
        );
        CREATE TABLE IF NOT EXISTS offshore_accounts (
            account_id  TEXT PRIMARY KEY,
            owner_name  TEXT,
            bank_loc    TEXT,
            entity_link TEXT
        );
        CREATE TABLE IF NOT EXISTS decrypted_comm (
            id          INTEGER PRIMARY KEY,
            sender      TEXT,
            body        TEXT
        );

        INSERT INTO employees VALUES
            ('E-10', 'Silas Thorne',   'Manager', '192.168.1.10', 'ACTIVE'),
            ('E-44', 'Arthur Pendelton','IT Lead', '192.168.1.44', 'ACTIVE'),
            ('E-50', 'Elena Rostova',  'Security','192.168.1.50', 'ACTIVE'),
            ('EXT-9', 'Julian Vane',   'Broker',  'UNKNOWN',      'N/A');

        INSERT INTO vault_logs VALUES
            (1, '01:00:00', 'HEARTBEAT', 'OK',       'SYSTEM'),
            (2, '02:14:00', 'ACCESS',    'OVERRIDE', 'E-44'),
            (3, '03:00:00', 'HEARTBEAT', 'OK',       'SYSTEM');

        INSERT INTO terminal_traffic VALUES
            (10, '02:12:30', '192.168.1.10', 'LOBBY_LIGHTS', 'ON'),
            (11, '02:13:00', '192.168.1.44', 'CAM-04',       'DISABLED'),
            (12, '02:15:00', '192.168.1.44', 'VAULT_GATE',   'CLOSE');

        INSERT INTO camera_grid VALUES
            ('CAM-03', 'Lobby',       'ONLINE',   '02:13:00'),
            ('CAM-04', 'Server Room', 'DISABLED', '02:13:00'),
            ('CAM-05', 'Vault Int',   'ONLINE',   '02:13:00');

        INSERT INTO access_badges VALUES
            ('B-101', 'E-10', 'STANDARD', '2046-11-01'),
            ('B-444', 'E-44', 'MASTER',   '02:12:05-12-00'),
            ('B-505', 'E-50', 'SECURITY', '2047-01-15');

        INSERT INTO wire_transfers VALUES
            ('TX-101', '02:20:00', 5000,       'Global_Rentals', 'NORMAL'),
            ('TX-102', '02:22:15', 2400000000, 'Ouroboros Holdings', 'CRITICAL'),
            ('TX-103', '02:30:00', 150000,     'Vane_Syndicate', 'FEE');

        INSERT INTO offshore_accounts VALUES
            ('ACC-88', 'Julian Vane', 'Cayman', 'Ouroboros Holdings'),
            ('ACC-99', 'Ghost_Node',  'Zurich', 'Vane_Syndicate');

        INSERT INTO decrypted_comm VALUES
            (1, 'Arthur', 'The Santiago vault is open. Initiate the Ouroboros routing now. Julian, I want my cut in the Zurich wallet.');
        )SQL";
    }

    void check_unlocks(GameState& state, const std::string& sql, const QueryResult& result) override {
        std::string up = GameState::upper(sql);

        if (GameState::sql_has(up, "VAULT_LOGS") && GameState::result_has_cell(result, "OVERRIDE"))
            state.unlock_table("camera_grid");

        if (GameState::sql_has(up, "CAMERA_GRID") && GameState::result_has_cell(result, "DISABLED"))
            state.unlock_table("access_badges");

        if (GameState::sql_has(up, "TERMINAL_TRAFFIC") && GameState::result_has_cell(result, "192.168.1.44"))
            state.unlock_table("wire_transfers");

        if (GameState::sql_has(up, "WIRE_TRANSFERS") && GameState::result_has_cell(result, "2400000000"))
            state.unlock_table("offshore_accounts");

        if (GameState::sql_has(up, "OFFSHORE_ACCOUNTS") && GameState::result_has_cell(result, "Julian Vane"))
            state.unlock_table("decrypted_comm");
    }

    void add_context_narrative(GameState& state, const std::string& sql, const QueryResult& result) override {
        std::string up = GameState::upper(sql);
        
        if (GameState::sql_has(up, "VAULT_LOGS") && GameState::result_has_cell(result, "OVERRIDE")) {
            state.push_narrative(NarrativeType::MONOLOGUE, "An override at 02:14. Someone forced the digital locks without a physical key.");
        }

        if (GameState::sql_has(up, "CAMERA_GRID") && GameState::result_has_cell(result, "DISABLED")) {
            state.push_narrative(NarrativeType::MONOLOGUE, "Camera 04 went dark right before the breach. The thief knew exactly where the blind spots were.");
        }

        if (GameState::sql_has(up, "EMPLOYEES") && GameState::result_has_cell(result, "Arthur Pendelton")) {
            state.push_narrative(NarrativeType::MONOLOGUE, "Pendelton. The IT Lead. He has the keys to the kingdom... and the logs.");
        }

        if (GameState::sql_has(up, "WIRE_TRANSFERS") && GameState::result_has_cell(result, "2400000000")) {
            state.push_narrative(NarrativeType::MONOLOGUE, "2.4 Billion. That's not a transfer; it's an extinction event for this bank.");
        }
    }

    bool try_accuse(GameState& state, const std::string& name) override {
        if (name == "ARTHUR PENDELTON" || name == "PENDELTON" || name == "E-44" || name == "ARTHUR") {
            state.app().status = GameStatus::CASE_SOLVED;
            state.push_narrative(NarrativeType::SUCCESS, "CASE CLOSED. Arthur Pendelton arrested for high-level digital bank robbery.");
            return true;
        } 
        else if (name == "JULIAN VANE" || name == "VANE" || name == "EXT-9" || name == "JULIAN") {
            state.app().status = GameStatus::CASE_SOLVED;
            state.push_narrative(NarrativeType::SUCCESS, "CASE CLOSED. Julian Vane flagged for international money laundering.");
            return true;
        }

        state.app().accuse.wrong_timer = 4.f;
        state.app().accuse.wrong_feedback = "'" + name + "' is not supported by the evidence. Focus on the terminal IP and the MASTER clearance.";
        state.push_narrative(NarrativeType::ALERT, "Wrong. " + state.app().accuse.wrong_feedback);
        return false;
    }

    std::vector<std::string> get_flag_keywords() override {
        return {"OVERRIDE", "192.168.1.44", "2400000000", "MASTER", "Julian Vane"};
    }

    bool can_accuse(const GameState& state) override {
        int found = 0;
        for (auto& c : state.clues()) { if (c.discovered) found++; }
        return found >= 8;
    }
    
    virtual bool seed_data(Database& db) override {
        auto result = db.run_query(get_sql_script());
        return !result.is_error;
    }
};