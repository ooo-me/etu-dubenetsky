// ============================================================================
// Классификатор
// Язык: C++20
// Описание: Класс для работы с иерархическим классификатором
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include <vector>
#include <memory>

namespace TariffSystem::Model {

/**
 * Классификатор компонентов системы
 * 
 * Представляет иерархическую структуру классов объектов
 * (услуги, тарифы, заказы и т.д.)
 */
class Classifier {
public:
    /**
     * Конструктор
     * @param id Уникальный идентификатор
     * @param code Код класса
     * @param name Наименование класса
     * @param level Уровень в иерархии
     */
    Classifier(Integer id, String code, String name, Integer level = 0)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , level_(level)
        , parentId_(std::nullopt)
    {}

    // Геттеры
    Integer getId() const { return id_; }
    const String& getCode() const { return code_; }
    const String& getName() const { return name_; }
    Integer getLevel() const { return level_; }
    std::optional<Integer> getParentId() const { return parentId_; }
    const std::optional<String>& getNote() const { return note_; }

    // Сеттеры
    void setParentId(Integer parentId) { parentId_ = parentId; }
    void setNote(String note) { note_ = std::move(note); }

    /**
     * Добавление дочернего класса
     * @param child Дочерний класс
     */
    void addChild(std::shared_ptr<Classifier> child) {
        children_.push_back(child);
        child->setParentId(id_);
    }

    /**
     * Получение всех дочерних классов
     * @return Вектор дочерних классов
     */
    const std::vector<std::shared_ptr<Classifier>>& getChildren() const {
        return children_;
    }

    /**
     * Проверка, является ли класс листовым (не имеет дочерних)
     * @return true, если класс листовой
     */
    bool isLeaf() const {
        return children_.empty();
    }

    /**
     * Проверка, является ли класс корневым
     * @return true, если класс корневой
     */
    bool isRoot() const {
        return !parentId_.has_value();
    }

private:
    Integer id_;                                           // Уникальный идентификатор
    String code_;                                          // Код класса
    String name_;                                          // Наименование
    Integer level_;                                        // Уровень в иерархии
    std::optional<Integer> parentId_;                      // ID родительского класса
    std::optional<String> note_;                           // Примечание
    std::vector<std::shared_ptr<Classifier>> children_;    // Дочерние классы
};

} // namespace TariffSystem::Model
